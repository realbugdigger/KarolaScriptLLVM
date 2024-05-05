#pragma once

#include <llvm/ADT/StringRef.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/Host.h>

//#include <mlir/Dialect/StandardOps/IR/Ops.h>
#include <mlir/Dialect/CommonFolders.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/Pass/PassManager.h>

#include "KarolaScriptNamespace.h"

enum CompilationPhase {
    KSIR,
    MLIR, // Lowered KSIR to other dialects
    LIR,  // Lowered to the llvm ir dialect
    IR,   // Lowered to the LLVMIR itself
    NoOptimization,
    O1,
    O2,
    O3
};

class KarolaScriptContext {
public:
    llvm::LLVMContext llvmContext;
    mlir::MLIRContext mlirContext;
    mlir::PassManager pm;
    std::string targetTriple;

    /// Insert the given `ns` into the context. The Context object is
    /// the owner of all the namespaces. The `ns` will overwrite any
    /// namespace with the same name.
    void insertNS(std::shared_ptr<KarolaScriptNamespace> ns);

    /// Sets the n ame of the current namespace in the context and return
    /// a boolean indicating the status of this operation. The operation
    /// will fail if the namespace does not exist in the namespace table.
    bool setCurrentNS(llvm::StringRef ns_name);

    /// Return the current namespace that is being processed at the moment
    std::shared_ptr<KarolaScriptNamespace> getCurrentNS();

    /// Lookup the namespace with the give name in the current context and
    /// return a shared pointer to it or a `nullptr` in it doesn't exist.
    std::shared_ptr<KarolaScriptNamespace> getNS(llvm::StringRef ns_name);

    KarolaScriptContext()
            : pm(&mlirContext), targetPhase(CompilationPhase::NoOptimization) {
        mlirContext.getOrLoadDialect<serene::slir::SereneDialect>();
        mlirContext.getOrLoadDialect<mlir::StandardOpsDialect>();
        // TODO: Get the crash report path dynamically from the cli
        // pm.enableCrashReproducerGeneration("/home/lxsameer/mlir.mlir");

        // TODO: Set the target triple with respect to the CLI args
        targetTriple = llvm::sys::getDefaultTargetTriple();
    };

    /// Set the target compilation phase of the compiler. The compilation
    /// phase dictates the behavior and the output type of the compiler.
    void setOperationPhase(CompilationPhase phase);

    CompilationPhase getTargetPhase() { return targetPhase; };
    int getOptimizationLevel();

private:
    CompilationPhase targetPhase;

    // The namespace table. Every namespace that needs to be compiled has
    // to register itself with the context and appear on this table.
    // This table acts as a cache as well.
    std::map<std::string, std::shared_ptr<KarolaScriptNamespace>> namespaces;

    // Why string vs pointer? We might rewrite the namespace and
    // holding a pointer means that it might point to the old version
    std::string current_ns;

};

/// Creates a new context object. Contexts are used through out the compilation
/// process to store the state
std::unique_ptr<KarolaScriptContext> makeKarolaScriptContext();
