#include <iostream>

#include "../lexer/lexer.h"
#include "../interpreter/RuntimeError.h"

//    In a full-featured language implementation, you will likely have multiple ways errors get displayed: on stderr, in an IDE’s error window, logged to a file, etc.
//    You don’t want that code smeared all over your scanner and Parser.

//    Ideally, we would have an actual abstraction, some kind of “ErrorReporter” interface that gets passed to the scanner and Parser so that we can swap out different reporting strategies.




// It will be good to implement error message like this

//    Error: Unexpected "," in argument list.
//
//    15 | function(first, second,);
//                               ^-- Here.



class ErrorReporter {
private:
    ErrorReporter(){}
public:
    static void error(int line, const char* message);
    static void error(Token token, const char* message);
    static void runtimeError(RuntimeError& error);
    static void warning(const char* message);
private:
    static void report(int line, const char* where, const char* message);
    static void reportWarning(const char* message);
};