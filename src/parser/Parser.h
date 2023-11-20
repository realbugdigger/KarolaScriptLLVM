#pragma once

#include <memory>
#include <vector>
#include <stdexcept>

#include "../lexer/Token.h"
#include "Expr.h"
#include "Stmt.h"

class Parser {
private:
    std::vector<Token> tokens;
    int current = 0; // next token eagerly waiting to be parsed  ---> currently considered token

    class ParseError : public std::runtime_error
    {
        public:
            ParseError() : std::runtime_error("") {}
    };

private:
    Token previous() {
        return tokens.at(current - 1);
    }

    Token peek() {
        return tokens.at(current);
    }

    bool isAtEnd() {
        return peek().type == TOKEN_EOF;
    }

    Token advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    bool check(TokenType type) {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    Token consume(TokenType type, const std::string& message) {
        if (check(type)) return advance();

        throw error(peek(), message);
    }

    std::runtime_error error(const Token& token, const std::string& message) {
        if (token.type == TOKEN_EOF) {
            return std::runtime_error(std::to_string(token.line) + " at end" + message);
        } else {
            return std::runtime_error(std::to_string(token.line) + " at '" + token.start + "'" + message);
        }
    }

    void synchronize() {
        advance();

        while (!isAtEnd()) {
            if (previous().type == TOKEN_SEMICOLON) return;

            switch (peek().type) {
                case TOKEN_CLAZZ:
                case TOKEN_FUNCT:
                case TOKEN_LET:
                case TOKEN_FOR:
                case TOKEN_IF:
                case TOKEN_WHILE:
                case TOKEN_KONSOLE:
                case TOKEN_RETURN:
                    return;
            }

            advance();
        }
    }

    bool match(const std::initializer_list<TokenType>& types) {
        for (auto type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }

    std::shared_ptr<Expr> expression() {
        return assignment();
    }

    std::shared_ptr<Expr> assignment() {
        std::shared_ptr<Expr> expr = commaExpression();

        if (match({ TOKEN_EQUAL })) {
            Token equals = previous();
            std::shared_ptr<Expr> value = assignment();

            auto variable = dynamic_cast<Variable*>(expr.get());
            auto get = dynamic_cast<Get*>(expr.get());
            if (variable != nullptr) {
                Token name = variable->m_VariableName;
                return std::make_shared<Assign>(name, value);
            }
            if (get != nullptr) {
                return std::make_shared<Set>(get->m_Object, get->m_Name, value);
            }

            error(equals, "Invalid assignment target.");
        }

        // if you don’t hit an `=`, report an error if the left-hand side isn’t a valid expression.
//        error(previous(), "Invalid expression. Did you try to perform assignment?"); // previous() ????

        // consume(SEMICOLON, "Expect ';' after expression."); ??????????

        return expr;
    }

    std::shared_ptr<Expr> commaExpression() {
        std::shared_ptr<Expr> expr = orSmt();

        while (match({TOKEN_COMMA})) {
            expr = orSmt();
        }

        return expr;
    }

    std::shared_ptr<Expr> orSmt() {
        std::shared_ptr<Expr> expr = andSmt();

        while (match({ TOKEN_OR })) {
            Token operation = previous();
            std::shared_ptr<Expr> right = andSmt();
            expr = std::make_shared<Logical>(expr, operation, right);
        }

        return expr;
    }

    std::shared_ptr<Expr> andSmt() {
        std::shared_ptr<Expr> expr = ternaryExpression();

        while (match({ TOKEN_AND })) {
            Token operation = previous();
            std::shared_ptr<Expr> right = ternaryExpression();
            expr = std::shared_ptr<Expr>(new Logical(expr, operation, right));
        }

        return expr;
    }

    std::shared_ptr<Expr> ternaryExpression() {
        std::shared_ptr<Expr> expr = equality();

        if (match({TOKEN_QUESTION_MARK})) {
            std::shared_ptr<Expr> trueExpr = equality();
            consume(TOKEN_COLON, "Expect ':' after ? in ternary operator.");
            std::shared_ptr<Expr> falseExpr = ternaryExpression();
            expr = std::make_shared<Ternary>(expr, trueExpr, falseExpr);
        }

        return expr;
    }

    std::shared_ptr<Expr> equality() {
        std::shared_ptr<Expr> expr = comparison();

        while (match({ TOKEN_BANG_EQUAL, TOKEN_EQUAL_EQUAL })) {
            Token operation = previous();
            std::shared_ptr<Expr> right = comparison();
            expr = std::make_shared<Binary>(expr, operation, right);
        }

        return expr;
    }

    std::shared_ptr<Expr> comparison() {
        std::shared_ptr<Expr> expr = term();

        while (match({ TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL })) {
            Token operation = previous();
            std::shared_ptr<Expr> right = term();
            expr = std::make_shared<Binary>(expr, operation, right);
        }
        return expr;
    }

    std::shared_ptr<Expr> term() {
        std::shared_ptr<Expr> expr = factor();

        while (match({TOKEN_MINUS, TOKEN_PLUS})) {
            Token operator_ = previous();
            std::shared_ptr<Expr> right = factor();
            expr = std::make_shared<Binary>(expr, operator_, right);
        }

        return expr;
    }

    std::shared_ptr<Expr> factor() {
        std::shared_ptr<Expr> expr = unary();

        while (match({TOKEN_SLASH, TOKEN_STAR})) {
            Token operator_ = previous();
            std::shared_ptr<Expr> right = unary();
            expr = std::make_shared<Binary>(expr, operator_, right);
        }

        return expr;
    }

    std::shared_ptr<Expr> unary() {
        if (match({TOKEN_BANG, TOKEN_MINUS})) {       //unaryOperator
            Token operator_ = previous();
            std::shared_ptr<Expr> right = unary();
            return std::make_shared<Unary>(operator_, right);
        }

        // this if block can probably go to primary() but checked first
        if (match({TOKEN_BANG_EQUAL, TOKEN_EQUAL_EQUAL, TOKEN_GREATER,
                   TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL,
                   TOKEN_MINUS, TOKEN_PLUS, TOKEN_SLASH, TOKEN_STAR})) {         //binaryOperator
            error(previous(), "Binary operator without left-hand operand.");
            expression();                   // Discard a right-hand operand
        }

        return call();
    }

    std::shared_ptr<Expr> finishCall(std::shared_ptr<Expr> callee) {
        std::vector<std::shared_ptr<Expr>> arguments;
        if (!check(TOKEN_RIGHT_PAREN)) {
            do {
                if (arguments.size() >= 255) {
                    error(peek(), "Cannot have more than 255 arguments.");
                }

                if (match({TOKEN_FUNCT}))
                    arguments.push_back(anonymousFunction());
                else
                    arguments.push_back(ternaryExpression());
            } while (match({ TOKEN_COMMA }));
        }

        Token paren = consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");

        return std::make_shared<Call>(callee, paren, arguments);
    }

    std::shared_ptr<Expr> call() {
        std::shared_ptr<Expr> expr = primary();

        while (true) {
            if (match({ TOKEN_LEFT_PAREN })) {
                expr = finishCall(expr);
            } else if (match({ TOKEN_DOT })) {
                Token name = consume(TOKEN_IDENTIFIER, "Expect property m_ClassName after '.'.");
                expr = std::make_shared<Get>(name, expr);
            } else {
                break;
            }
        }
        return expr;
    }

    std::shared_ptr<Expr> anonymousFunction() {
        consume(TOKEN_LEFT_PAREN, "Expect '(' after 'fun'.");
        std::vector<Token> parameters;
        if (!check(TOKEN_RIGHT_PAREN)) {
            do {
                if (parameters.size() >= 255) {
                    error(peek(), "Can't have more than 255 parameters.");
                }

                parameters.push_back(consume(TOKEN_IDENTIFIER, "Expect parameter m_ClassName."));
            } while (match({TOKEN_COMMA}));
        }
        Token errorToken = consume(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
        consume(TOKEN_LEFT_BRACE, "Expect '{' before fun body.");
        std::vector<std::shared_ptr<Stmt>> body = block();

        if (body.empty())
            throw error(errorToken, "Anonymous function is declared without being used.");

        return std::make_shared<AnonFunction>(parameters, body);
    }

    std::shared_ptr<Expr> primary() {
        if (match({ TOKEN_FALSE })) {
            return std::make_shared<Literal>(false);
        }
        if (match({ TOKEN_TRUE })) {
            return std::make_shared<Literal>(true);
        }
        if (match({ TOKEN_NULL })) {
            return std::make_shared<Literal>(std::any{}); // 0 ???
        }

        if (match({ TOKEN_NUMBER })) {
            return std::make_shared<Literal>(std::strtod(previous().lexeme.c_str(), nullptr));
        }
        if (match({ TOKEN_STRING })) {
            return std::make_shared<Literal>(previous().lexeme);
        }

        if (match({ TOKEN_SUPER })) {
            Token keyword = previous();
            consume(TOKEN_DOT, "Expect '.' after 'super'.");
            Token method = consume(TOKEN_IDENTIFIER,"Expect m_Superclass method m_ClassName.");
            return std::make_shared<Super>(keyword, method);
        }

        if (match({ TOKEN_THIS })) {
            return std::make_shared<This>(previous());
        }

        if (match({ TOKEN_IDENTIFIER })) {
            return std::make_shared<Variable>(previous());
        }

        if (match({ TOKEN_LEFT_PAREN })) {
            std::shared_ptr<Expr> expr = expression();
            consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
            return std::make_shared<Grouping>(expr);
        }

        throw error(peek(), "Expect expression.");
    }

    std::shared_ptr<Stmt> forStatement() {
        consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");

        // declaring or initializing variable
        std::shared_ptr<Stmt> initializer;
        if (match({ TOKEN_SEMICOLON })) {
            initializer = nullptr;
        } else if (match({ TOKEN_LET })) {
            initializer = letDeclaration();
        } else {
            initializer = expressionStatement();
        }

        // the condition
        std::shared_ptr<Expr> condition = nullptr;
        if (!check(TOKEN_SEMICOLON)) {
            condition = expression();
        }
        consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");

        // the incrementation
        std::shared_ptr<Expr> increment = nullptr;
        if (!check(TOKEN_RIGHT_PAREN)) {
            increment = expression();
        }
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");

        std::shared_ptr<Stmt> body = statement();

        if (increment != nullptr) {
            std::vector<std::shared_ptr<Stmt>> stmts;
            stmts.push_back(body);
            stmts.push_back(std::make_shared<Expression>(increment));
            body = std::make_shared<Block>(stmts);
        }

        if (condition == nullptr) {
            condition = std::make_shared<Literal>(true);
        }
        body = std::make_shared<While>(condition, body);

        if (initializer != nullptr) {
            std::vector<std::shared_ptr<Stmt>> stmts;
            stmts.push_back(initializer);
            stmts.push_back(body);
            body = std::make_shared<Block>(stmts);
        }

        return body;
    }

    std::shared_ptr<Stmt> ifStatement() {
        consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
        std::shared_ptr<Expr> condition = expression();
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after if condition.");

        std::shared_ptr<Stmt> thenBranch = statement();
        std::shared_ptr<Stmt> elseBranch = nullptr;
        if (match({ TOKEN_ELSE })) {
            elseBranch = statement();
        }

        return std::make_shared<If>(condition, thenBranch, elseBranch);
    }

    std::shared_ptr<Stmt> konsoleStatement() {
        std::shared_ptr<Expr> value = expression();
        consume(TOKEN_SEMICOLON, "Expect ';' after value.");
        std::shared_ptr<Stmt> print = std::make_shared<Print>(value);
        return print;
    }

    std::shared_ptr<Stmt> returnStatement() {
        Token keyword = previous();
        std::shared_ptr<Expr> value;
        if (!check(TOKEN_SEMICOLON)) {
            value = expression();
        }

        consume(TOKEN_SEMICOLON, "Expect ';' after return value.");
        return std::make_shared<Return>(keyword, value);
    }

    std::shared_ptr<Stmt> whileStatement() {
        consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
        std::shared_ptr<Expr> condition = expression();
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");
        std::shared_ptr<Stmt> body = statement();

        return std::make_shared<While>(condition, body);
    }

    std::shared_ptr<Stmt> breakStatement() {
        Token keyword = previous();

//        if (!insideLoop()) {
//            error(keyword, "Must be inside a loop to use 'break'.");
//            return null;
//        }

        consume(TOKEN_SEMICOLON, "Expect ';' after 'break'.");
        return std::make_shared<Break>(keyword);
    }

    std::vector<std::shared_ptr<Stmt>> block() {
        std::vector<std::shared_ptr<Stmt>> statements;

        while (!check(TOKEN_RIGHT_BRACE) && !isAtEnd()) {
            statements.push_back(declaration());
        }

        consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
        return statements;
    }

    std::shared_ptr<Stmt> expressionStatement() {
        std::shared_ptr<Expr> expr = expression();
        consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
        std::shared_ptr<Stmt> expression = std::make_shared<Expression>(expr);
        return expression;
    }

    std::shared_ptr<Stmt> statement() {
        if (match({ TOKEN_FOR })) {
            return forStatement();
        }
        if (match({ TOKEN_IF })) {
            return ifStatement();
        }
        if (match({ TOKEN_KONSOLE })) {
            return konsoleStatement();
        }
        if (match({ TOKEN_RETURN })) {
            return returnStatement();
        }
        if (match({ TOKEN_WHILE })) {
            return whileStatement();
        }
        if (match({ TOKEN_BREAK })) {
            return breakStatement();
        }
        if (match({ TOKEN_LEFT_BRACE })) {
            return std::make_shared<Block>(block());
        }
        return expressionStatement();
    }

    std::shared_ptr<Stmt> letDeclaration() {
        Token name = consume(TOKEN_IDENTIFIER, "Expect variable m_ClassName.");
        std::shared_ptr<Expr> initializer;
        if (match( {TOKEN_EQUAL} )) {
            initializer = expression();
        }

        consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
        std::shared_ptr<Stmt> var = std::make_shared<Let>(name, initializer);
        return var;
    }

    std::shared_ptr<Function> function(const std::string& kind) {
        Token name = consume(TOKEN_IDENTIFIER, "Expect " + kind + " m_ClassName.");
        consume(TOKEN_LEFT_PAREN, "Expect '(' after " + kind + " m_ClassName.");
        std::vector<Token> parameters;
        if (!check(TOKEN_RIGHT_PAREN)) {
            do {
                if (parameters.size() >= 255) {
                    error(peek(), "Cannot have more than 255 parameters.");
                }
                parameters.push_back(consume(TOKEN_IDENTIFIER, "Expect parameter m_ClassName."));
            } while (match({ TOKEN_COMMA }));
        }
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
        consume(TOKEN_LEFT_BRACE, "Expect '{' before " + kind + " body.");
        std::vector<std::shared_ptr<Stmt>> body = block();
        return std::make_shared<Function>(name, parameters, body);
    }

    std::shared_ptr<Stmt> clazzDeclaration() {
        Token name = consume(TOKEN_IDENTIFIER, "Expect class m_ClassName.");

        std::shared_ptr<Variable> superclass;
        if (match( {TOKEN_LESS} )) {
            consume(TOKEN_IDENTIFIER, "Expect m_Superclass m_ClassName.");
            superclass = std::make_shared<Variable>(previous());
        }

        consume(TOKEN_LEFT_BRACE, "Expect '{' before class body.");

        std::vector<std::shared_ptr<Function>> methods;
        std::vector<std::shared_ptr<Function>> classMethods;
        while (!check(TOKEN_RIGHT_BRACE) && !isAtEnd()) {
            if (match({TOKEN_CLAZZ})) {
                classMethods.push_back(function("method"));
            } else {
                methods.push_back(function("method"));
            }
        }

        consume(TOKEN_RIGHT_BRACE, "Expect '}' after class body.");

        return std::make_shared<Class>(name, superclass, methods, classMethods);
    }

    std::shared_ptr<Stmt> declaration() {
        try
        {
            if (match({TokenType::TOKEN_CLAZZ}))
                return clazzDeclaration();
            if (match({TokenType::TOKEN_LET}))
                return letDeclaration();
            if (match({TokenType::TOKEN_FUNCT}))
                return function("function");

            return statement();
        }
        catch (ParseError&)
        {
            synchronize();
            return nullptr;
        }
    }

public:
    explicit Parser(const std::vector<Token> &tokens) : tokens(tokens) {}

    std::vector<std::shared_ptr<Stmt>> parse() {
        std::vector<std::shared_ptr<Stmt>> statements;
        while (!isAtEnd()) {
            statements.push_back(declaration());
        }

        return statements;
    }
};