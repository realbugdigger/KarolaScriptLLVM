#pragma once

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Value.h>

#include "../interpreter/Interpreter.h"
#include "../interpreter/Environment.h"
#include "../parser/Expr.h"
#include "../parser/Stmt.h"

class CodeGenVisitor : public ExprVisitor<llvm::Value*>, public StmtVisitor {
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
private:
    std::shared_ptr<Environment> globals;
    std::shared_ptr<Environment> environment;
    // Contains the number of "hops" between the current environment and the environment where the variable referenced by Expr* is stored
    std::unordered_map<const Expr*, int> localsDistances;

    // The EnvironmentGuard class is used to manage the CodeGenVisitor's environment stack. It follows the
    // RAII technique, which means that when an instance of the class is created, a copy of the current
    // environment is stored, and the current environment is moved to the new one. If a runtime error is
    // encountered and the CodeGenVisitor needs to unwind the stack and return to the previous environment,
    // the EnvironmentGuard class's destructor is called, which swaps the resources back to the previous
    // environment.
    class EnvironmentGuard
    {
    private:
        CodeGenVisitor& codeGenVisitor;
        std::shared_ptr<Environment> previous_env;
    public:
        EnvironmentGuard(CodeGenVisitor& genVisitor, std::shared_ptr<Environment> enclosing_env)
                : codeGenVisitor{genVisitor}, previous_env{genVisitor.environment} {
            genVisitor.environment = std::move(enclosing_env);
        }

        ~EnvironmentGuard() {
            codeGenVisitor.environment = std::move(previous_env);
        }
    };

    Interpreter& m_Interpreter;
public:
    CodeGenVisitor(Interpreter& m_Interpreter);

    /* This function unpacks every UniqueStmtPtr into a raw pointer and then executes it. This is because the "Visitor" does not
     * own the dynamically allocated statement objects, it only operates on them, so it should use raw pointers instead of a
     * smart pointer to signal that it does not own and has no influence over the lifetime of the objects.
     * */
    void generate(std::vector<UniqueStmtPtr>& statements);

    llvm::Value* visitSetExpr(Set& expr) override;
    llvm::Value* visitLogicalExpr(Logical& expr) override;
    llvm::Value* visitLiteralExpr(Literal& expr) override;
    llvm::Value* visitGroupingExpr(Grouping& expr) override;
    llvm::Value* visitCallExpr(Call& expr) override;
    llvm::Value* visitAnonFunctionExpr(AnonFunction& expr) override;
    llvm::Value* visitGetExpr(Get& expr) override;
    llvm::Value* visitAssignExpr(Assign& expr) override;
    llvm::Value* visitBinaryExpr(Binary& expr) override;
    llvm::Value* visitThisExpr(This& expr) override;
    llvm::Value* visitSuperExpr(Super& expr) override;
    llvm::Value* visitUnaryExpr(Unary& expr) override;
    llvm::Value* visitVariableExpr(Variable& expr) override;
    llvm::Value* visitTernaryExpr(Ternary& expr) override;

    void visitExpressionStmt(Expression& stmt) override;
    void visitReturnStmt(Return& stmt) override;
    void visitBreakStmt(Break& stmt) override;
    void visitLetStmt(Let& stmt) override;
    void visitWhileStmt(While& stmt) override;
    void visitIfStmt(If& stmt) override;
    void visitBlockStmt(Block& stmt) override;
    void visitFunctionStmt(Function& stmt) override;
    void visitPrintStmt(Print& stmt) override;
    void visitClazzStmt(Class& clazzStmt) override;

private:
    void compile(Stmt* stmt);

    void moduleInit();

    void setupExternFunctions();

    void saveModuleToFile(const std::string& fileName);

    llvm::Value* gen(Object object);

    llvm::Value* lookupVariable(const Token& identifier, const Expr* variableExpr);


    // Returns size of a type in bytes
    size_t getTypeSize(llvm::Type* type_);

    /**
     * Extracts var or parameter name considering type.
     */
    std::string extractVarName(Token token);

    /**
     * Extracts var or parameter type with i32 as default.
     */
    llvm::Type* extractVarType(/*const Expr& expr*/);

    llvm::Value* allocVar(const std::string& name, llvm::Type* type_);

    llvm::Value* compileFunction(const Function* functExpr);

    llvm::Function* createFunction(const std::string& fnName, llvm::FunctionType* fnType);

    // Create function prototype (defines the function but not the body)
    llvm::Function* createFunctionPrototype(const std::string& fnName, llvm::FunctionType* fnType);

    void createFunctionBlock(llvm::Function* fn);

    /**
     * Creates a basic block. If the `fn` is passed, the block is automatically appended to the parent function.
     * Otherwise, the block should later be appended manually via
     * fn->getBasicBlockList().push_back(block);
     */
    llvm::BasicBlock* createBasicBlock(const std::string& name, llvm::Function* fn = nullptr);
};