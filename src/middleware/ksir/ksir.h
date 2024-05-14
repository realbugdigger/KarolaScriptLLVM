#pragma once

#include "../KarolaScriptContext.h"

#include <memory>

#include <llvm/IR/Module.h>


std::unique_ptr<llvm::Module> compileToLLVMIR(KarolaScriptContext &ctx, mlir::ModuleOp &module);
