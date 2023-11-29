#include <string>
#include <iostream>

#include "lexer/lexer.h"
#include "parser/Parser.h"
#include "interpreter/Interpreter.h"
#include "interpreter/Resolver.h"
#include "interpreter/RuntimeError.h"

Interpreter interpreter = Interpreter();
Resolver resolver = Resolver(interpreter);

// Both the prompt and the file runner are thin wrappers around this core function
static void run(const char* program) {
    initLexer(program);
    std::vector<Token> tokenList = scanTokens();
    auto* parser = new Parser(tokenList);
    std::vector<UniqueStmtPtr> statements = parser->parse();

    // Stop if there was a syntax error.
    if (hadParseError)
        return;
//        exit(65);

    resolver.resolve(statements);

    // Stop if there was a resolution error.
    if (hadResolutionError)
        return;

    try {
        interpreter.interpret(statements);
    } catch (const RuntimeError &exception) {
    }

    free(parser);
}

static void repl() {
    char line[1024];
    for (;;) {
        printf("ks> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        run(line);

        tokens = {};
        hadParseError = false;
        hadResolutionError = false;
    }
}

static char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == nullptr) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == nullptr) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

static void runFile(const char* path) {
    char* source = readFile(path);

    run(source);

    tokens = {};
    hadParseError = false;
    hadResolutionError = false;

    free(source);
}

int main(int argc, const char* argv[]) {

    // 1. Reference for used error exit codes: https://man.freebsd.org/cgi/man.cgi?query=sysexits&apropos=0&sektion=0&manpath=FreeBSD+4.3-RELEASE&format=html

    // 2. This interpreter is pretty bare bones. It doesn't have stuff like interactive debugger and static analyzer.

    if (argc == 2) {
        repl();
    } else if (argc == 1) {
//        runFile(argv[1]);
//        runFile("/home/marko/compilers/KarolaScript/src/resources/code1.ks");
//        runFile("/home/marko/compilers/KarolaScript/src/resources/code2.ks");
//        runFile("/home/marko/compilers/KarolaScript/src/resources/code3.ks");
//        runFile("/home/marko/compilers/KarolaScript/src/resources/code4.ks");
//        runFile("/home/marko/compilers/KarolaScript/src/resources/code5.ks");
//        runFile("/home/marko/compilers/KarolaScript/src/resources/code6.ks");
        runFile("/home/marko/compilers/KarolaScript/src/resources/code7.ks");
    } else {
        fprintf(stderr, "Usage: ks [filePath]\n");
        exit(64);
    }

    return 0;
}
