#pragma once

#include <any>
#include <map>
#include <stack>

#include "../parser/Stmt.h"
#include "../parser/Expr.h"
//#include "Environment.h"
#include "Interpreter.h"

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

    Interpreter& m_Interpreter;

    std::stack<FunctionType> func_stack;

    std::vector<std::unordered_map<std::string, bool>> scopes;
    std::vector<std::unordered_map<std::string, bool>> usages;
public:
    explicit Resolver(Interpreter& interpreter) : m_Interpreter{interpreter} {
        func_stack.push(FunctionType::FUNCTION_NONE);
    }

    void resolve(std::vector<std::shared_ptr<Stmt>>& statements) {
        for (auto& stmt : statements) {
            resolve(stmt);
        }
    }

    void resolve(const std::shared_ptr<Stmt>& stmt) {
        stmt->accept(*this);
    }

    void resolve(const std::shared_ptr<Expr>& expr) {
        expr->accept(*this);
    }

    std::any visitSetExpr(Set& expr) override {
        resolve(expr.m_Value);
        resolve(expr.m_Object);
        return nullptr;
    }

    std::any visitLogicalExpr(Logical& expr) override {
        resolve(expr.m_Left);
        resolve(expr.m_Right);
        return nullptr;
    }

    std::any visitLiteralExpr(Literal& expr) override {
        return nullptr;
    }

    std::any visitGroupingExpr(Grouping& expr) override {
        resolve(expr.m_Expression);
        return nullptr;
    }

    std::any visitCallExpr(Call& expr) override {
        resolve(expr.m_Callee);

        for (const auto& argument : expr.m_Arguments) {
            resolve(argument);
        }
        return nullptr;
    }

    std::any visitAnonFunctionExpr(AnonFunction& expr) override {
        resolveFunction(expr);
        return nullptr;
    }

    std::any visitGetExpr(Get& expr) override {
        resolve(expr.m_Object);
        return nullptr;
    }

    std::any visitAssignExpr(Assign& expr) override {
        resolve(expr.m_Value);
        resolveLocal(expr, expr.m_Name);
        return nullptr;
    }

    std::any visitBinaryExpr(Binary& expr) override {
        resolve(expr.m_Left);
        resolve(expr.m_Right);
        return nullptr;
    }

    std::any visitThisExpr(This& expr) override {
        if (currentClass == CLASS_NONE) {
            lox::error(expr.m_Keyword.line, "Cannot use 'this' outside of a class.");
            return nullptr;
        }
        resolveLocal(expr, expr.m_Keyword);
        return nullptr;
    }

    std::any visitSuperExpr(Super& expr) override {
        if (currentClass == CLASS_NONE) {
            lox::error(expr.m_Keyword.line, "Cannot use 'super' outside of a class.");
        } else if (currentClass != SUBCLASS) {
            lox::error(expr.m_Keyword.line, "Cannot use 'super' in a class with no m_Superclass.");
        }
        resolveLocal(expr, expr.m_Keyword);
        return nullptr;
    }

    std::any visitUnaryExpr(Unary& expr) override {
        resolve(expr.m_Right);
        return nullptr;
    }

    std::any visitVariableExpr(Variable& expr) override {
        if (!scopes.empty()) {
            auto last = scopes.back();
            auto searched = last.find(expr.m_VariableName.lexeme);
            if (searched != last.end() && !searched->second) {
                lox::error(expr.m_VariableName.line, "Cannot read local variable in its own initializer.");
            }
        }
        resolveLocal(expr, expr.m_VariableName);
        return nullptr;
    }

    std::any visitTernaryExpr(Ternary& expr) override {
        resolve(expr.m_Expr);
        resolve(expr.m_TrueExpr);
        resolve(expr.m_FalseExpr);
        return nullptr;
    }

    void visitExpressionStmt(Expression& stmt) override {
        resolve(stmt.m_Expression);
    }

    void visitReturnStmt(Return& stmt) override {
        if (currentFunction == FUNCTION_NONE) {
            lox::error(stmt.m_Keyword.line, "Cannot return from top-level code.");
        }

        if (stmt.m_Value != nullptr) {
            if (currentFunction == INITIALIZER) {
                lox::error(stmt.m_Keyword.line, "Cannot return a m_Value from an initializer.");
            }
            resolve(stmt.m_Value);
        }
    }

    void visitBreakStmt(Break& stmt) override {
        // If not in a nested loop, add a new error.
        if (loop_nesting_level == 0) {
            Error::addError(stmt.m_Keyword, "Can't break outside of a loop.");
        }
        //throw BreakException(stmt.m_Keyword);
//        throw BreakError(stmt.m_Keyword);
    }

    void visitLetStmt(Let& stmt) override {
        declare(stmt.m_Name);
        if (stmt.m_Initializer != nullptr) {
            resolve(stmt.m_Initializer);
        }
        define(stmt.m_Name);
    }

    void visitWhileStmt(While& stmt) override {
        resolve(stmt.m_Condition);
        resolve(stmt.m_Body);
    }

    void visitIfStmt(If& stmt) override {
        resolve(stmt.m_Condition);
        resolve(stmt.m_ThenBranch);
        if (stmt.m_ElseBranch != nullptr) {
            resolve(stmt.m_ElseBranch);
        }
    }

    void visitBlockStmt(Block& stmt) override {
        beginScope();
        resolve(stmt.m_Statements);
        endScope();
    }

    void visitFunctionStmt(Function& stmt) override {
        declare(stmt.m_Name);
        define(stmt.m_Name);
        resolveFunction(stmt, FUNCTION);
    }

    void visitPrintStmt(Print& stmt) override {
        resolve(stmt.m_Expression);
    }

    void visitClazzStmt(Class& stmt) override {
        ClassType enclosingClass = currentClass;
        currentClass = CLASS;

        declare(stmt.m_Name);
        define(stmt.m_Name);

        if (stmt.m_Superclass != nullptr &&
            stmt.m_Name.lexeme == stmt.m_Superclass->name.lexeme) {
            lox::error(stmt.m_Superclass->name.line, "A class cannot inherit from itself.");
        }

        if (stmt.m_Superclass != nullptr) {
            currentClass = SUBCLASS;
            resolve(stmt.m_Superclass);
        }

        if (stmt.m_Superclass != nullptr) {
            beginScope();
            auto backed = scopes.back();
            backed["super"] = true;
            scopes.pop_back();
            scopes.emplace_back(backed);
        }

        beginScope();
        auto back = scopes.back();
        back["this"] = true;
        scopes.pop_back();
        scopes.emplace_back(back);

        for (const auto& method : stmt.m_Methods) {
            FunctionType declaration = METHOD;
            if (method->m_Name.lexeme == "init") {
                declaration = INITIALIZER;
            }
            resolveFunction(*method, declaration);
        }

        endScope();

        if (stmt.m_Superclass != nullptr) endScope();

        currentClass = enclosingClass;
    }

private:
    void beginScope() {
        std::unordered_map<std::string, bool> s;
        scopes.push_back(s);
    }

    void endScope() {
        scopes.pop_back();
    }

    void resolveFunction(Function& function, FunctionType type) {
        FunctionType enclosingFunction = currentFunction;
        currentFunction = type;

        beginScope();
        for (const Token& param : function.m_Params) {
            declare(param);
            define(param);
        }
        resolve(function.m_Body);
        endScope();
        currentFunction = enclosingFunction;
    }

    void resolveFunction(AnonFunction& function) {
        beginScope();
        for (const Token& param : function.m_Params) {
            declare(param);
            define(param);
        }
        resolve(function.m_Body);
        endScope();
    }

    void declare(const Token& name) {
        if (scopes.empty()) return;

        // Get the innermost scope.
        std::unordered_map<std::string, bool> scope = scopes.back();

        // Don't allow the same variable declaration more than once.
        if (scope.find(name.lexeme) != scope.end()) {
            lox::error(name.line, "Variable with this m_Name already declared in this scope.");
        }


        // The m_Value associated with a key in the scope map represents whether we have completed
        // resolving the initializer for that variable.
        //scope.try_emplace(identifier.lexeme, false);

        scope[name.lexeme] = false;
        scopes.pop_back();
        scopes.emplace_back(scope);
    }

    void define(const Token& name) {
        if (scopes.empty()) return;

        // Indicates that the variable has been fully initialized by setting the m_Value to true.
        //scopes.back()[identifier.lexeme] = true;

        // Get the innermost scope.
        std::unordered_map<std::string, bool> scope = scopes.back();

        scope[name.lexeme] = true;
        scopes.pop_back();
        scopes.emplace_back(scope);
    }

//    void resolveLocal(std::shared_ptr<Expr>& expr, const Token& m_Name) {
//        for (int i = scopes.size() - 1; i >= 0; i--) {
//            auto searched = scopes[i].find(m_Name.lexeme);
//            if (searched != scopes[i].end()) {
//                m_Interpreter.resolve(expr, scopes.size() - 1 - i);
//                return;
//            }
//        }
//        // Not found. Assume it is global.
//    }

    void resolveLocal(const Expr& expr, const Token& identifier) {
        if (scopes.empty())
            return;

        // Look for a variable starting from the innermost scope.
        for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope)
        {
            // If variable is found, then we resolve it.
            if (scope->find(identifier.lexeme) != scope->end())
            {
                m_Interpreter.resolve(expr, std::distance(scopes.rbegin(), scope));
                return;
            }
        }
        // ... If never found, we can assume that the variable is global.
    }
};