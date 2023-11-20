cmake_minimum_required(VERSION 3.26)
project(KarolaScript)

set(CMAKE_CXX_STANDARD 17)

add_executable(KarolaScript src/main.cpp
        src/lexer/lexer.h
        src/lexer/lexer.cpp
        src/ErrorReporter.h
        src/lexer/Token.h
        src/parser/Parser.h
        src/parser/Expr.h
        src/parser/Stmt.h
        src/interpreter/Interpreter.h
        src/interpreter/Environment.h
        src/interpreter/RuntimeError.h
        src/interpreter/Resolver.h
        src/interpreter/KarolaScriptInstance.h
        src/interpreter/KarolaScriptClass.h
        src/interpreter/KarolaScriptFunction.h
        src/interpreter/KarolaScriptCallable.h
        src/util/Object.h)
