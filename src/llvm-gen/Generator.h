#pragma once

#include <string>
#include <memory>
#include <iostream>

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>

class Generator {
private:
    // Currently compiling function
    llvm::Function* fn;

    /**
     * Global LLVM context.
     * It owns and manages the core "global" data of LLVM core
     * infrastructure, including the type and constant unique tables.
     */
    std::unique_ptr<llvm::LLVMContext> ctx;

    /**
     * A Module instance is used to store all the information related to an
     * LLVM module. Modules are the top level container of all other LLVM
     * Intermediate Representation objects. Each module directly contains a
     * list of global variables, a list of functions, a list of libraries
     * (or other modules) this module depends on, a symbol table, and various
     * data about target's characteristics.
     *
     * A module maintains a GlobalList object that is used to hold all
     * constant references to global variables in the module. When a global
     * variable is destroyed, it should have no entries in the GlobalList.
     * The main container class for the LLVM Intermediate Representation.
     */
    std::unique_ptr<llvm::Module> module;

    /**
     * IR Builder.
     *
     * This provides a uniform API for creating instructions and inserting
     * them into a basic block: either at the end of a BasicBlock, or at a
     * specific iterator location in a block.
     */
    std::unique_ptr<llvm::IRBuilder<>> builder;
public:
    Generator();

    void generate();
private:
    void moduleInit();

    // Saves IR to file.
    void saveModuleToFile(const std::string& fileName);

    // main compile loop
    llvm::Value* gen();

    llvm::GlobalVariable* createGlobalVar(const std::string& name, llvm::Constant* init);

    // Define external functions (from libc++)
    void setupExternalFunctions();

    llvm::Function* createFunction(const std::string& fnName, llvm::FunctionType* fnType);

    // Create function prototype (defines the function but not the body)
    llvm::Function* createFunctionPrototype(const std::string& fnName, llvm::FunctionType* fnType);

    void createFunctionBlock(llvm::Function* fn);

    /**
     * Creates a basic block. If the `fn` is passed, the block is automatically appended to the parent function.
     * Otherwise, the block should later be appended manually via
     * fn->getBasicBlockList().push_back(block);
     */
    llvm::BasicBlock* createBasicBlock(const std::string name, llvm::Function* fn = nullptr);

    void compile();
};