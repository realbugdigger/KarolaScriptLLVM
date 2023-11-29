cmake_minimum_required(VERSION 3.26)
project(KarolaScript)

set(CMAKE_CXX_STANDARD 17)

add_executable(KarolaScript src/main.cpp
        src/lexer/lexer.h
        src/lexer/lexer.cpp
        src/util/ErrorReporter.h
        src/lexer/Token.h
        src/parser/Parser.h
        src/parser/Expr.h
        src/parser/Stmt.h
        src/interpreter/Interpreter.h
        src/interpreter/Environment.h
        src/interpreter/RuntimeError.h
        src/interpreter/Resolver.h
        src/interpreter/KarolaScriptClass.h
        src/interpreter/KarolaScriptFunction.h
        src/interpreter/KarolaScriptCallable.h
        src/util/Object.h
        src/interpreter/KarolaScriptClass.cpp
        src/interpreter/KarolaScriptFunction.cpp
        src/util/Object.cpp
        src/interpreter/Environment.cpp
        src/interpreter/Interpreter.cpp
        src/util/common.h
        src/interpreter/Resolver.cpp
        src/util/Utils.h
        src/util/Utils.cpp
        src/interpreter/ks_stdlib/StdLibFunctions.h
        src/interpreter/ks_stdlib/StdLibFunctions.cpp
        src/interpreter/KarolaScriptAnonFunction.h
        src/interpreter/KarolaScriptAnonFunction.cpp
        src/util/ErrorReporter.cpp
        src/parser/Parser.cpp
)
