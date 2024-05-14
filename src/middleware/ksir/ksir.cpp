#include <mlir/Target/LLVMIR/Export.h>
#include <llvm/Support/TargetSelect.h>
#include <mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h>
#include <mlir/ExecutionEngine/OptUtils.h>
#include "ksir.h"

std::unique_ptr<llvm::Module> compileToLLVMIR(KarolaScriptContext &ctx,
                                              mlir::ModuleOp &module) {
    // Register the translation to LLVM IR with the MLIR context.
    mlir::registerLLVMDialectTranslation(ctx.mlirContext);

    // Convert the module to LLVM IR in a new LLVM IR context.
    auto llvmModule = mlir::translateModuleToLLVMIR(module, ctx.llvmContext);
    if (!llvmModule) {
        // TODO: Return a Result type instead
        llvm::errs() << "Failed to emit LLVM IR\n";
        throw std::runtime_error("Failed to emit LLVM IR\n");
    }

    // Initialize LLVM targets.
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    // TODO: replace this call with our own version of setupTargetTriple
//    mlir::ExecutionEngine::setupTargetTriple(llvmModule.get());

    /// Optionally run an optimization pipeline over the llvm module.
    auto optPipeline = mlir::makeOptimizingTransformer(
            /*optLevel=*/ctx.getOptimizationLevel(), /*sizeLevel=*/0,
            /*targetMachine=*/nullptr);
    if (auto err = optPipeline(llvmModule.get())) {
        llvm::errs() << "Failed to optimize LLVM IR " << err << "\n";
        throw std::runtime_error("Failed to optimize LLVM IR");
    }

    return llvmModule;
};
