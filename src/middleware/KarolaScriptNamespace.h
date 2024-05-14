#pragma once

#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/Module.h>
#include <llvm/ADT/Optional.h>

#include <mlir/IR/Builders.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/OwningOpRef.h>
#include <mlir/IR/Value.h>
#include <mlir/Support/LogicalResult.h>

#include "KarolaScriptContext.h"
#include "Environment.h"
#include "../util/common.h"

/// KarolaScript's namespaces are the unit of compilation.
/// Any code that needs to be compiled has to be in a namespace.
/// The official way to create a new namespace is to use the `makeNamespace` function.
class KarolaScriptNamespace {
private:
    KarolaScriptContext& ctx;

    mlir::StringRef name;
    llvm::Optional<std::string> filename;

    /// The root environment of the namespace on the semantic analysis phase.
    /// Which is a mapping from names to AST nodes ( no evaluation ).
    Environment<std::string, UniqueExprPtr> semanticEnv;

    /// Th root environment to store the MLIR value during the IR generation phase.
    Environment<llvm::StringRef, mlir::Value> symbolTable;

    std::vector<UniqueStmtPtr>& ast;

public:
    KarolaScriptNamespace(KarolaScriptContext &ctx, llvm::StringRef ns_name,
            llvm::Optional<llvm::StringRef> filename);

    KarolaScriptContext &getContext();

    /// Generate and return a MLIR ModuleOp tha contains the IR of the namespace
    /// with respect to the compilation phase
    mlir::OwningOpRef<mlir::ModuleOp> generate();

    /// Compile the namespace to a llvm module.
    /// It will call the `generate` method of the namespace to generate the IR.
    std::unique_ptr<llvm::Module> compileToLLVM();

    void addNamespaceAst(std::vector<UniqueStmtPtr>& astNodes);

    std::vector<UniqueStmtPtr>& getTree();

    /// Run all the passes specified in the context on the given MLIR ModuleOp.
    mlir::LogicalResult runPasses(mlir::ModuleOp &m);

    /// Dumps the namespace with respect to the compilation phase
    void dump();

    ~KarolaScriptNamespace();

};
