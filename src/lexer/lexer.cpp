#include <cstring>

#include "../util/ErrorReporter.h"
#include "lexer.h"

typedef struct {
    const char* start;      // marks the beginning of the current lexeme being scanned
    const char* current;    // points to the current character being looked at (considered character)
    int line;               // what line the current lexeme is on for error reporting
} Lexeme;

Lexeme lexeme;
std::string program;

void initLexer(const char* source) {
    lexeme.start = source;
    lexeme.current = source;
    lexeme.line = 1;

    program = source;
}

static bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static bool isAtEnd() {
    return *lexeme.current == '\0';
}

static bool isAtEndNext() {
    return *(lexeme.current + 1) == '\0';
}

static char advance() {
    lexeme.current++;
    return lexeme.current[-1];
}

static char peek() {
    return *lexeme.current;
}

static char peekNext() {
    if (isAtEnd()) return '\0';
    return lexeme.current[1];
}

static bool match(char expected) {
    if (isAtEnd()) return false;
    if (*lexeme.current != expected) return false;
    lexeme.current++;
    return true;
}

static Token makeToken(TokenType type) {
    Token token{};
    token.type = type;
    token.start = lexeme.start;
    token.length = (int)(lexeme.current - lexeme.start);
    token.line = lexeme.line;
    token.lexeme = "";
    return token;
}

static Token makeToken(TokenType type, const std::string& literal) {
    Token token{};
    token.type = type;
    token.start = lexeme.start;
    token.length = (int)(lexeme.current - lexeme.start);
    token.line = lexeme.line;
    token.lexeme = literal;
    return token;
}


// doesn't support nested comments !!!
static void commentBlock() {
    while (peek() != '*' && peekNext() != '/' && !isAtEndNext()) {
        if (peek() == '\n') lexeme.line++;
        advance();
    }

    if (isAtEndNext()) {
        ErrorReporter::error(lexeme.line, "Unterminated comment block.");
        return;
    }

    // The closing */.
    advance();
    advance();
}

static void skipWhitespace() {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                lexeme.line++;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    // A comment goes until the end of the line.
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
    if (lexeme.current - lexeme.start == start + length &&
        memcmp(lexeme.start + start, rest, length) == 0) {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {
    switch (lexeme.start[0]) {
        case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
        case 'b': return checkKeyword(1, 4, "reak", TOKEN_BREAK);
        case 'c':
            if (lexeme.current - lexeme.start > 1) {
                switch (lexeme.start[1]) {
                    case 'l': return checkKeyword(2, 3, "azz", TOKEN_CLAZZ);
                    case 'o': return checkKeyword(2, 5, "nsole", TOKEN_KONSOLE);
                }
            }
            break;
        case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
        case 'f':
            if (lexeme.current - lexeme.start > 1) {
                switch (lexeme.start[1]) {
                    case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
                    case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
                    case 'u': return checkKeyword(2, 3, "nct", TOKEN_FUNCT);
                }
            }
            break;
        case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
        case 'n': return checkKeyword(1, 2, "ull", TOKEN_NULL);
        case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
        case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
        case 's':
            if (lexeme.current - lexeme.start > 1) {
                switch (lexeme.start[1]) {
                    case 'u': return checkKeyword(2, 3, "per", TOKEN_SUPER);
                    case 't': return checkKeyword(2, 4, "atic", TOKEN_STATIC);
                }
            }
            break;
        case 't': return checkKeyword(1, 3, "rue", TOKEN_TRUE);
        case 'l': return checkKeyword(1, 2, "et", TOKEN_LET);
        case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }

    return TOKEN_IDENTIFIER;
}

static Token identifier() {
    while (isAlpha(peek()) || isDigit(peek())) advance();

    TokenType tokenType = identifierType();
    if (tokenType == TOKEN_IDENTIFIER) {
        std::string literal(lexeme.start, (int)(lexeme.current - lexeme.start));
        return makeToken(tokenType, literal);
    }
    return makeToken(tokenType);
}

static Token number() {
    while (isDigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume the ".".
        advance();

        while (isDigit(peek())) advance();
    }

    std::string literal(lexeme.start, (int)(lexeme.current - lexeme.start));
    return makeToken(TOKEN_NUMBER, literal);
}

static Token string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') lexeme.line++;
        advance();
    }

    if (isAtEnd()) ErrorReporter::error(lexeme.line, "Unterminated string.");

    // The closing quote.
    advance();

    std::string literal(lexeme.start, (int)(lexeme.current - lexeme.start));
    return makeToken(TOKEN_STRING, literal);
}

Token scanToken() {
    skipWhitespace();
    lexeme.start = lexeme.current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();
    if (isAlpha(c)) return identifier();
    if (isDigit(c)) return number();

    switch (c) {
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TOKEN_LEFT_BRACE);
        case '}': return makeToken(TOKEN_RIGHT_BRACE);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case ',': return makeToken(TOKEN_COMMA);
        case '.': return makeToken(TOKEN_DOT);
        case '-': return makeToken(TOKEN_MINUS);
        case '+': return makeToken(TOKEN_PLUS);
        case '/': return makeToken(TOKEN_SLASH);
        case '*': return makeToken(TOKEN_STAR);
        case '?': return makeToken(TOKEN_QUESTION_MARK);
        case ':': return makeToken(TOKEN_COLON);
        case '!':
            return makeToken(
                    match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return makeToken(
                    match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<':
            return makeToken(
                    match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return makeToken(
                    match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '"': return string();
    }

    ErrorReporter::error(lexeme.line, "Unexpected character.");
}

std::vector<Token> scanTokens() {
    while (!isAtEnd()) {
        // We are at the beginning of the next lexeme.
        lexeme.start = lexeme.current;
        tokens.push_back(scanToken());
    }

    tokens.push_back({TOKEN_EOF});
    return tokens;
}