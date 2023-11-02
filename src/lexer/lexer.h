#pragma once

#include <vector>

enum TokenType {

    // Single-character tokens.
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR, TOKEN_QUESTION_MARK, TOKEN_COLON,

    // One or two character tokens.
    TOKEN_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL,

    // Literals.
    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

    // Keywords.
    TOKEN_AND, TOKEN_CLAZZ, TOKEN_ELSE, TOKEN_FALSE, TOKEN_FUNCT, TOKEN_FOR, TOKEN_IF, TOKEN_NULL, TOKEN_OR,
    TOKEN_KONSOLE, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS, TOKEN_TRUE, TOKEN_LET, TOKEN_WHILE,
    TOKEN_BREAK,

    TOKEN_EOF
};

typedef struct Token {
    TokenType type;
    const char* start;
    int length;
    int line;
} Token;

inline std::vector<Token> tokens;

void initLexer(const char* source);
Token scanToken();
std::vector<Token> scanTokens();