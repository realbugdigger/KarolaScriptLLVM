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
            return std::runtime_error(std::to_string(token.line) + " at '" + token.lexeme + "'" + message);
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
        std::shared_ptr<Expr> expr = orSmt();
        if (match({ TOKEN_EQUAL })) {
            Token equals = previous();
            std::shared_ptr<Expr> value = assignment();

            auto variable = dynamic_cast<Variable<Object>*>(expr.get());
            auto get = dynamic_cast<Get<Object>*>(expr.get());
            if (variable != nullptr) {
                Token name = variable->name;
                return std::shared_ptr<Expr>(
                        new Assign<Object>(name, value));
            }
            if (get != nullptr) {
                return shared_ptr<Expr<Object>>(
                        new Set<Object>(get->object, get->name, value));
            }
            error(equals, "Invalid assignment target.");
        }
        return expr;
    }

    std::shared_ptr<Expr> orSmt() {
        std::shared_ptr<Expr> expr = andSmt();
        while (match({ TOKEN_OR })) {
            Token operation = previous();
            std::shared_ptr<Expr> right = andSmt();
            expr = std::shared_ptr<Expr>(
                    new Logical<Object>(expr, operation, right));
        }
        return expr;
    }

    std::shared_ptr<Expr> andSmt() {
        std::shared_ptr<Expr> expr = equality();
        while (match({ TOKEN_AND })) {
            Token operation = previous();
            std::shared_ptr<Expr> right = equality();
            expr = shared_ptr<Expr<Object>>(new Logical<Object>(expr, operation, right));
        }
        return expr;
    }

    std::shared_ptr<Stmt> statement() {
        if (match({ TOKEN_FOR })) {
            return forStatement();
        }
        if (match({ TOKEN_IF })) {
            return ifStatement();
        }
        if (match({ TOKEN_KONSOLE })) {
            return printStatement();
        }
        if (match({ TOKEN_RETURN })) {
            return returnStatement();
        }
        if (match({ TOKEN_WHILE })) {
            return whileStatement();
        }
        if (match({ TOKEN_LEFT_BRACE })) {
            return shared_ptr<Stmt>(new Block(block()));
        }
        return expressionStatement();
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
        return std::shared_ptr<Stmt>(new If(condition, thenBranch, elseBranch));
    }

    std::shared_ptr<Stmt> printStatement() {
        std::shared_ptr<Expr> value = expression();
        consume(TOKEN_SEMICOLON, "Expect ';' after value.");
        std::shared_ptr<Stmt> print(new Print(value));
        return print;
    }

    std::shared_ptr<Stmt> returnStatement() {
        Token keyword = previous();
        std::shared_ptr<Expr> value;
        if (!check(TOKEN_SEMICOLON)) {
            value = expression();
        }
        consume(TOKEN_SEMICOLON, "Expect ';' after return value.");
        return std::shared_ptr<Stmt>(new Return(keyword, value));
    }

    std::shared_ptr<Stmt> whileStatement() {
        consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
        std::shared_ptr<Expr> condition = expression();
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");
        std::shared_ptr<Stmt> body = statement();
        return std::shared_ptr<Stmt>(new While(condition, body));
    }

    std::vector<std::shared_ptr<Stmt>> block() {
        std::vector<std::shared_ptr<Stmt>> statements;

        while (!check(TOKEN_RIGHT_BRACE) && !isAtEnd()) {
            statements.push_back(declaration());
        }
        consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
        return statements;
    }

    std::shared_ptr<Stmt> letDeclaration() {
        Token name = consume(TOKEN_IDENTIFIER, "Expect variable name.");
        std::shared_ptr<Expr> initializer;
        if (match( {TOKEN_EQUAL} )) {
            initializer = expression();
        }

        consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
        std::shared_ptr<Stmt> var(new Let<void>(name, initializer));
        return var;
    }

    std::shared_ptr<Stmt> function(const std::string& kind) {
        auto identifier = consume(TokenType::TOKEN_IDENTIFIER, "Expect " + kind + " name.");
        void_cast(consume(TokenType::TOKEN_LEFT_PAREN, "Expect '(' after " + kind + " name."));
        std::vector<Token> params;
        if (!check(TokenType::TOKEN_RIGHT_PAREN))
        {
            do
            {
                if (params.size() > 254)
                {
                    error(peek(), "Can't exceed more than 254 parameters.");
                }

                auto parameter = consume(TokenType::TOKEN_IDENTIFIER, "Expect parameter name.");
                params.emplace_back(parameter);
            } while (match({TokenType::TOKEN_COMMA}));
        }

        void_cast(consume(TokenType::TOKEN_RIGHT_PAREN, "Expect ')' after parameters."));
        void_cast(consume(TokenType::TOKEN_LEFT_BRACE, "Expect '{' before " + kind + " body."));

        auto body = block();

        return std::make_shared<Function<>>(std::move(identifier), std::move(params), std::move(body));
    }

    std::shared_ptr<Function> function(const std::string& kind) {
        Token name = consume(TOKEN_IDENTIFIER, "Expect " + kind + " name.");
        consume(TOKEN_LEFT_PAREN, "Expect '(' after " + kind + " name.");
        std::vector<Token> parameters;
        if (!check(TOKEN_RIGHT_PAREN)) {
            do {
                if (parameters.size() >= 255) {
                    error(peek(), "Cannot have more than 255 parameters.");
                }
                parameters.push_back(consume(TOKEN_IDENTIFIER, "Expect parameter name."));
            } while (match({ TOKEN_COMMA }));
        }
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
        consume(TOKEN_LEFT_BRACE, "Expect '{' before " + kind + " body.");
        std::vector<std::shared_ptr<Stmt>> body = block();
        return std::shared_ptr<Function>(new Function(name, parameters, body));
    }



    std::shared_ptr<Stmt> clazzDeclaration() {
        Token name = consume(TOKEN_IDENTIFIER, "Expect class name.");

        std::shared_ptr<Let> superclass;
        if (match( {TOKEN_LESS} )) {
            consume(TOKEN_IDENTIFIER, "Expect superclass name.");
            superclass = std::shared_ptr<Variable<Object>>(new Variable<Object>(previous()));
        }
        consume(TOKEN_LEFT_BRACE, "Expect '{' before class body.");

        std::vector<std::shared_ptr<Function>> methods;
        while (!check(TOKEN_RIGHT_BRACE) && !isAtEnd()) {
            methods.push_back(function("method"));
        }
        consume(TOKEN_RIGHT_BRACE, "Expect '}' after class body.");

        return std::shared_ptr<Stmt>(new Class(name, superclass, methods));
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