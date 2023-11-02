cmake_minimum_required(VERSION 3.26)
project(KarolaScript)

set(CMAKE_CXX_STANDARD 17)

add_executable(KarolaScript src/main.cpp
        src/lexer/lexer.h
        src/lexer/lexer.cpp
        src/ErrorReporter.h)