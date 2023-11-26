#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <stack>

#include "../parser/Expr.h"
#include "../util/Object.h"
#include "../parser/Stmt.h"
#include "../util/common.h"

inline bool hadResolutionError = false;

class Resolver : public StmtVisitor, public ExprVisitor<Object> {
private:
    enum FunctionType {
        FUNCTION_NONE,
        FUNCTION,
        METHOD,
        INITIALIZER
    };
    enum ClassType {
        CLASS_NONE,
        CLASS,
        SUBCLASS,
    };
    ClassType currentClass = CLASS_NONE;
    FunctionType currentFunction = FUNCTION_NONE;

    std::unordered_map<const Expr*, int> distances;

    int loopNestingLevel = 0;

    std::vector<std::unordered_map<std::string, bool>> scopes;
    std::vector<std::unordered_map<std::string, int>> usages;
public:
    void resolve(const std::vector<UniqueStmtPtr> &statements);
    void resolve(Stmt* stmt);
    void resolve(Expr* expr);

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
    void visitClazzStmt(Class& stmt) override;

private:
    void beginScope();
    void endScope();

    void increaseUsage(const Token& name);

    void resolveFunction(Function& function, FunctionType type);
    void resolveFunction(AnonFunction& function);
    void resolveLocal(const Expr& expr, const Token& identifier);

    void declare(const Token& name);
    void define(const Token& name);
};