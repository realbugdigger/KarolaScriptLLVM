#pragma once

#include <llvm/IR/LLVMContext.h>
#include <mlir/IR/MLIRContext.h>

enum CompilationPhase {
    KSIR,
    MLIR,
    LIR,
    IR,
    NoOptimization,
    O1,
    O2,
    O3
};

class KarolaScriptContext {
public:
    llvm::LLVMContext llvmContext;
    mlir::MLIRContext mlirContext;


};