#pragma once

#include <memory>
#include <vector>
#include <stdexcept>
#include <optional>

#include "../lexer/Token.h"
#include "Expr.h"
#include "Stmt.h"
#include "../util/ErrorReporter.h"

inline bool hadParseError = false;

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
    Token previous();
    Token peek();
    bool isAtEnd();
    Token advance();
    bool check(TokenType type);
    Token consume(TokenType type, const std::string& message);
    bool match(const std::initializer_list<TokenType>& types);

    ParseError error(const Token& token, const std::string& message);
    void synchronize();

    UniqueExprPtr expression();
    UniqueExprPtr assignment();
    UniqueExprPtr commaExpression();
    UniqueExprPtr orSmt();
    UniqueExprPtr andSmt();
    UniqueExprPtr ternaryExpression();
    UniqueExprPtr equality();
    UniqueExprPtr comparison();
    UniqueExprPtr term();
    UniqueExprPtr factor();
    UniqueExprPtr unary();
    UniqueExprPtr finishCall(UniqueExprPtr callee);
    UniqueExprPtr call();
    UniqueExprPtr anonymousFunction();
    UniqueExprPtr primary();
    UniqueStmtPtr forStatement();
    UniqueStmtPtr ifStatement();
    UniqueStmtPtr konsoleStatement();
    UniqueStmtPtr returnStatement();
    UniqueStmtPtr whileStatement();
    UniqueStmtPtr breakStatement();
    std::vector<UniqueStmtPtr> block();
    UniqueStmtPtr expressionStatement();
    UniqueStmtPtr statement();
    UniqueStmtPtr letDeclaration();
    std::unique_ptr<Function> function(const std::string& kind);
    UniqueStmtPtr clazzDeclaration();
    UniqueStmtPtr declaration();

public:
    explicit Parser(const std::vector<Token> &tokens) : tokens(tokens) {}

    std::vector<UniqueStmtPtr> parse() {
        std::vector<UniqueStmtPtr> statements;
        while (!isAtEnd()) {
            statements.push_back(declaration());
        }

        return statements;
    }
};