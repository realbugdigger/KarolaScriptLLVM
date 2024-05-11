#include "KarolaScriptNamespace.h"

KarolaScriptNamespace::KarolaScriptNamespace(KarolaScriptContext &ctx, llvm::StringRef ns_name,
                     llvm::Optional<llvm::StringRef> filename)
        : ctx(ctx), name(ns_name) {
    if (filename.hasValue()) {
        this->filename.emplace(filename.getValue().str());
    }
}

KarolaScriptContext &KarolaScriptNamespace::getContext() { return this->ctx; };

mlir::OwningOpRef<mlir::ModuleOp> KarolaScriptNamespace::generate() {
    mlir::OpBuilder builder(&ctx.mlirContext);
    // TODO: Fix the unknown location by pointing to the `ns` form
    auto module = mlir::ModuleOp::create(builder.getUnknownLoc(),
                                         llvm::Optional<llvm::StringRef>(name));

    // Walk the AST and call the `generateIR` function of each node.
    // Since nodes will have access to the a reference of the
    // namespace they can use the builder and keep adding more
    // operations to the module via the builder
    for (auto &x : getTree()) {
        x->generateIR(*this, module);
    }

    if (mlir::failed(mlir::verify(module))) {
        module.emitError("Can't verify the module");
        module.erase();
        return llvm::None;
    }

    if (mlir::failed(runPasses(module))) {
        // TODO: Report a proper error
        module.emitError("Failure in passes!");
        module.erase();
        return llvm::None;
    }

    return MaybeModuleOp(module);
}

std::unique_ptr<llvm::Module> KarolaScriptNamespace::compileToLLVM() {
    auto maybeModule = generate();

    if (!maybeModule) {
        NAMESPACE_LOG("IR generation failed for '" << name << "'");
        return llvm::None;
    }

    if (ctx.getTargetPhase() >= CompilationPhase::IR) {
        mlir::ModuleOp module = maybeModule.getValue().get();
        return MaybeModule(::ks::ksir::compileToLLVMIR(ctx, module));
    }

    return llvm::None;
}

mlir::LogicalResult KarolaScriptNamespace::runPasses(mlir::ModuleOp &m) {
    return ctx.pm.run(m);
}

