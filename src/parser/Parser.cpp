//#include <vector>
//
//#include "../lexer/Token.h"
//#include "Expr.h"
//// #include "Stmt.h"
//
//class Parser {
//private:
//    std::vector<Token> tokens;
//    int current = 0; // next token eagerly waiting to be parsed  ---> currently considered token
//
//    std::vector<Stmt> statements;
//
//private:
//    Token previous() {
//        return tokens.at(current - 1);
//    }
//
//    Token peek() {
//        return tokens.at(current);
//    }
//
//    bool isAtEnd() {
//        return peek().type == EOF;
//    }
//
//    Token advance() {
//        if (!isAtEnd()) current++;
//        return previous();
//    }
//
//    bool check(TokenType type) {
//        if (isAtEnd()) return false;
//        return peek().type == type;
//    }
//
//    Token consume(TokenType type, std::string message) {
//        if (check(type)) return advance();
//
//        throw error(peek(), message);
//    }
//
//    bool match(TokenType... types) {
//        for (TokenType type : types) {
//            if (check(type)) {
//                advance();
//                return true;
//            }
//        }
//
//        return false;
//    }
//
//public:
//    Parser(const std::vector<Token> &tokens) : tokens(tokens) {}
//
//    std::vector<Stmt> parse() {
//        while (!isAtEnd()) {
//            statements.push_back(declaration());
//        }
//
//        return statements;
//    }
//};