cmake_minimum_required(VERSION 3.16)
project(KarolaScript)

set(CMAKE_CXX_STANDARD 17)



set(LLVM_DIR "/home/marko/reserve/llvm-project/build/lib/cmake/llvm/")
set(MLIR_DIR "/home/marko/reserve/llvm-project/build/lib/cmake/mlir/")

find_package(LLVM REQUIRED CONFIG)
find_package(MLIR REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include_directories(${MLIR_INCLUDE_DIRS})
add_definitions(${MLIR_DEFINITIONS})



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
        src/llvm-gen/CodeGenVisitor.h
        src/llvm-gen/CodeGenVisitor.cpp
        src/llvm-gen/Environment.h
        src/llvm-gen/Environment.cpp
        src/llvm-gen/KarolaScriptNamespace.h
        src/llvm-gen/KarolaScriptContext.h
)



llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(KarolaScript ${llvm_libs})
target_link_libraries(KarolaScript MLIRIR)