#pragma once

#include <unordered_map>
#include <vector>
#include <cmath>

#include "Environment.h"
#include "../parser/Expr.h"
#include "../parser/Stmt.h"
#include "../util/Object.h"
#include "../util/common.h"

class Interpreter : public StmtVisitor, public ExprVisitor<Object> {
private:
    std::shared_ptr<Environment> globals;
    std::shared_ptr<Environment> environment;
    // Contains the number of "hops" between the current environment and the environment where the variable referenced by Expr* is stored
    std::unordered_map<const Expr*, int> localsDistances;

    // The EnvironmentGuard class is used to manage the interpreter's environment stack. It follows the
    // RAII technique, which means that when an instance of the class is created, a copy of the current
    // environment is stored, and the current environment is moved to the new one. If a runtime error is
    // encountered and the interpreter needs to unwind the stack and return to the previous environment,
    // the EnvironmentGuard class's destructor is called, which swaps the resources back to the previous
    // environment.
    class EnvironmentGuard
    {
        private:
            Interpreter& interpreter;
            std::shared_ptr<Environment> previous_env;
        public:
            EnvironmentGuard(Interpreter& interpreter, std::shared_ptr<Environment> enclosing_env)
                : interpreter{interpreter}, previous_env{interpreter.environment} {
                interpreter.environment = std::move(enclosing_env);
            }

            ~EnvironmentGuard() {
                interpreter.environment = std::move(previous_env);
            }
    };
public:
    Interpreter();

    /*This function unpacks every UniqueStmtPtr into a raw pointer and then executes it. This is because the Interpreter does not
     * own the dynamically allocated statement objects, it only operates on them, so it should use raw pointers instead of a
     * smart pointer to signal that it does not own and has no influence over the lifetime of the objects.
     * */
    void interpret(std::vector<UniqueStmtPtr>& statements);

    Object visitSetExpr(Set& expr) override;
    Object visitLogicalExpr(Logical& expr) override;
    Object visitLiteralExpr(Literal& expr) override;
    Object visitGroupingExpr(Grouping& expr) override;
    Object visitCallExpr(Call& expr) override;
    Object visitAnonFunctionExpr(AnonFunction& expr) override;
    Object visitGetExpr(Get& expr) override;
    Object visitAssignExpr(Assign& expr) override;
    Object visitBinaryExpr(Binary& expr) override;
    Object visitThisExpr(This& expr) override;
    Object visitSuperExpr(Super& expr) override;
    Object visitUnaryExpr(Unary& expr) override;
    Object visitVariableExpr(Variable& expr) override;
    Object visitTernaryExpr(Ternary& expr) override;

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

public:
    Object evaluate(Expr* expr);

    void execute(Stmt* stmt);

    void executeBlock(std::vector<UniqueStmtPtr>& statements, std::shared_ptr<Environment> enclosing_env);

    // KarolaScript follows Ruby’s simple rule: `false` and `null` are falsey, and everything else is truthy
    bool isTruthy(const Object& object) const;

    bool isEqual(const Object& lhs, const Object& rhs) const;

    void checkNumberOperand(const Token& op, const Object& operand) const;

    void checkNumberOperands(const Token& op, const Object& lhs, const Object& rhs) const;

    std::string stringify(const Object& object);

    Object lookupVariable(const Token& identifier, const Expr* variableExpr);

    void loadNativeFunctions();
};
