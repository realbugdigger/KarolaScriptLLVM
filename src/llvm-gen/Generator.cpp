//#include <iostream>

#include "Generator.h"

Generator::Generator() {
    moduleInit();
    setupExternalFunctions();
}

void Generator::generate() {

    // Compile to LLVM IR
    compile(/* std::vector<UniqueStmtPtr>& statements */);

    // Print generated code.
    module->print(llvm::outs(), nullptr);

    std::cout << "\n";

    // Save module IR to file:
    saveModuleToFile("./ks_llvm.ll");
}

void Generator::moduleInit() {
    // Open a new context and module.
    ctx = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("KarolaScript-LLVM", *ctx);

    // Create a new builder for the module.
    builder = std::make_unique<llvm::IRBuilder<>>(*ctx);
}

void Generator::saveModuleToFile(const std::string &fileName) {
    std::error_code errorCode;
    llvm::raw_fd_ostream outLL(fileName, errorCode);
    module->print(outLL, nullptr);
}

llvm::Value *Generator::gen(Object o) {
    if (o.isNumber())
        return builder->getInt32(o.getNumber());
    if (o.isString())
        return builder->CreateGlobalString(o.getString());
    if (o.isBoolean())
        return builder->getInt1(o.getBoolean());

//    auto str = builder->CreateGlobalString("Hello, world!\n");
//
//    // call to printf;
//    auto printfFn = module->getFunction("printf");
//
//    // args
//    std::vector<llvm::Value*> args{str};
//
//    return builder->CreateCall(printfFn, args);
}

llvm::GlobalVariable *Generator::createGlobalVar(const std::string &name, llvm::Constant *init) {
    module->getOrInsertGlobal(name, init->getType());
    auto variable = module->getNamedGlobal(name);
    variable->setAlignment(llvm::MaybeAlign(4));
    variable->setConstant(false);
    variable->setInitializer(init);
    return variable;
}

void Generator::setupExternalFunctions() {
    // i8* is substitute for char*, void*, etc
    auto bytePtrTy = builder->getInt8Ty()->getPointerTo();

    // int printf(const char* format, ...);
    module->getOrInsertFunction("printf", llvm::FunctionType::get(/* return type*/ builder->getInt32Ty(),
                                                                /* format arg */ bytePtrTy,
                                                                /* vararg  */ true
                                                        ));
}

llvm::Function *Generator::createFunction(const std::string &fnName, llvm::FunctionType *fnType) {
    // Function prototype might already be defined:
    auto fn = module->getFunction(fnName);

    // If not, allocate the function:
    if (fn == nullptr) {
        fn = createFunctionPrototype(fnName, fnType);
    }

    createFunctionBlock(fn);
    return fn;
}

llvm::Function *Generator::createFunctionPrototype(const std::string &fnName, llvm::FunctionType *fnType) {
    // Function prototype might already be defined:
    auto fn = llvm::Function::Create(fnType, llvm::Function::ExternalLinkage, fnName, *module);

    // Validate the generated code, checking for consistency.
    llvm::verifyFunction(*fn);
    return fn;
}

void Generator::createFunctionBlock(llvm::Function *fn) {
    auto entry = createBasicBlock("entry", fn);
    builder->SetInsertPoint(entry);
}

llvm::BasicBlock *Generator::createBasicBlock(const std::string name, llvm::Function *fn) {
    return llvm::BasicBlock::Create(*ctx, name, fn);
}

void Generator::compile() {
    // 1. Create main function
    fn = createFunction("main", llvm::FunctionType::get(/* return type*/ builder->getInt32Ty(),
                                                        /* vararg */ false));

//    createGlobalVar("KAROLA", builder->getInt32(1000));

    // 2. Create main body:
    gen(/* ast */);

    // Cast to i32 to return from main:
//        auto i32Result = builder->CreateIntCast(result, builder->getInt32Ty(), true);

//        builder->CreateRet(i32Result);
    builder->CreateRet(builder->getInt32(0));
}