#include "KarolaScriptContext.h"
#include "KarolaScriptNamespace.h"

void KarolaScriptContext::insertNS(std::shared_ptr<KarolaScriptNamespace> ns) {
    namespaces[ns->name] = ns;
}

std::shared_ptr<KarolaScriptNamespace> KarolaScriptContext::getNS(llvm::StringRef ns_name) {
    if (namespaces.count(ns_name.str())) {
        return namespaces[ns_name.str()];
    }

    return nullptr;
}

bool KarolaScriptContext::setCurrentNS(llvm::StringRef ns_name) {
    if (namespaces.count(ns_name.str())) {
        this->current_ns = ns_name;
        return true;
    }

    return false;
}

std::shared_ptr<KarolaScriptNamespace> KarolaScriptContext::getCurrentNS() {
    if (this->current_ns.empty() || !namespaces.count(this->current_ns)) {
        panic(*this, llvm::formatv("getCurrentNS: Namespace '{0}' does not exist",
                                   this->current_ns)
                .str());
    }

    return *namespaces[this->current_ns];
}

void KarolaScriptContext::setOperationPhase(CompilationPhase phase) {
    this->targetPhase = phase;

    if (phase == CompilationPhase::KSIR) {
        return;
    }

    if (phase >= CompilationPhase::MLIR) {
        pm.addPass(ks::passes::createSLIRLowerToMLIRPass());
    }

    if (phase >= CompilationPhase::LIR) {
        pm.addPass(ks::passes::createSLIRLowerToLLVMDialectPass());
    }
}

int KarolaScriptContext::getOptimizationLevel() {
    if (targetPhase <= CompilationPhase::NoOptimization) {
        return 0;
    }

    if (targetPhase == CompilationPhase::O1) {
        return 1;
    }
    if (targetPhase == CompilationPhase::O2) {
        return 2;
    }
    return 3;
}

