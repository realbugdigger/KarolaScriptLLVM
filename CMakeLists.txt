cmake_minimum_required(VERSION 3.16)
project(KarolaScript)

set(CMAKE_CXX_STANDARD 17)



set(LLVM_DIR "/home/marko/reserve/llvm-project/build/lib/cmake/llvm/")
set(MLIR_DIR "/home/marko/reserve/llvm-project/build/lib/cmake/mlir/")

# The path to mlir-tblgen binary
set(MLIR_TABLEGEN_EXE "/home/marko/reserve/llvm-project/build/tools/mlir/tools/mlir-tblgen")

# Define the paths to your source directories (mlir includes and lib)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/middleware/mlir)
set(INCLUDE_DIR ${SOURCE_DIR}/include)
set(LIB_DIR ${SOURCE_DIR}/lib)

find_package(LLVM REQUIRED CONFIG)
find_package(MLIR REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include_directories(${MLIR_INCLUDE_DIRS})
add_definitions(${MLIR_DEFINITIONS})


file(GLOB_RECURSE TD_FILES ${INCLUDE_DIR}/*.td)

file(GLOB_RECURSE INC_FILES ${INCLUDE_DIR}/*.inc)

foreach (TD_FILE ${TD_FILES})

    # Remove the suffix *.td from TD_FILE
    string(REGEX REPLACE "\.td$" "" BASE_FILE ${TD_FILE})

    # Construct the paths for the output `.inc` files
    set(OUT_CPP_INC_FILE ${BASE_FILE}.cpp.inc)
    set(OUT_H_INC_FILE ${BASE_FILE}.h.inc)

    # Create the custom command for running `mlir-tblgen`
    add_custom_command(OUTPUT "${OUT_H_INC_FILE}" "${OUT_CPP_INC_FILE}"
                        DEPENDS "${MLIR_TABLEGEN_EXE}" "${TD_FILE}"
                        COMMAND "${MLIR_TABLEGEN_EXE}" -gen-op-decls -I ${INCLUDE_DIR}
                            -I ${CMAKE_SOURCE_DIR}/llvm-project/llvm/include -I ${CMAKE_SOURCE_DIR}/llvm-project/mlir/include ${TD_FILE} -o ${OUT_H_INC_FILE} # check and confirm this twice !!!!!!!!!!
                        COMMAND "${MLIR_TABLEGEN_EXE}" -gen-op-decls -I ${INCLUDE_DIR}
                            -I ${CMAKE_SOURCE_DIR}/llvm-project/llvm/include -I ${CMAKE_SOURCE_DIR}/llvm-project/mlir/include ${TD_FILE} -o ${OUT_CPP_INC_FILE} # check and confirm this twice !!!!!!!!!!
                        COMMENT "Generating MLIR operation declarations and definitions for ${BASE_FILE}")

    list(APPEND INC_FILES ${OUT_H_INC_FILE} ${OUT_CPP_INC_FILE})

endforeach (TD_FILE)

## Add build rule for dialects here

### ...

#add_library(
##        MyDialect
##        MyPass
#        ${INC_FILES}
#        src/middleware/Environment.cpp
#)

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
        src/middleware/llvm-gen/CodeGenVisitor.h
        src/middleware/llvm-gen/CodeGenVisitor.cpp
        src/middleware/Environment.h
        src/middleware/Environment.cpp
        src/middleware/KarolaScriptNamespace.h
        src/middleware/KarolaScriptContext.h
        src/middleware/KarolaScriptNamespace.cpp
        src/middleware/KarolaScriptContext.cpp
        src/middleware/utils/precompiles.h
        src/middleware/ksir/ksir.h
        src/middleware/ksir/ksir.cpp
        src/middleware/mlir/lib/Dialect/test_dialect.h
        src/middleware/mlir/lib/Dialect/test_dialect.cpp
        src/middleware/mlir/lib/Dialect/Polynomial/PolyDialect.cpp
)



llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(KarolaScript ${llvm_libs})
target_link_libraries(KarolaScript MLIRIR)