#include "ErrorReporter.h"

void ErrorReporter::error(int line, const char* message) {
    report(line, "", message);
}

void ErrorReporter::error(Token token, const char* message) {
    if (token.type == TOKEN_EOF) {
        report(token.line, " at end", message);
    } else {
        report(token.line, "", message);
    }
}

void ErrorReporter::runtimeError(RuntimeError& error) {
    fprintf(stderr, "[!] Runtime Error: \"%s\".\n", error.getMessage().c_str());
}

void ErrorReporter::warning(const char* message) {
    reportWarning(message);
}

void ErrorReporter::report(int line, const char* where, const char* message) {
    fprintf(stderr, "[%d] Error %s: %s\n", line, where, message);
}

void ErrorReporter::reportWarning(const char* message) {
    fprintf(stderr, "[*] Warning: %s\n", message);
}