#include "Resolver.h"

#include <iostream>

#include "RuntimeError.h"
#include "../ErrorReporter.h"
#include "../lexer/Token.h"
#include "Interpreter.h"


Resolver::Resolver(Interpreter& interpreter) : m_Interpreter(interpreter) {}

void Resolver::resolve(const std::vector<UniqueStmtPtr> &statements) {
    for (auto& stmt : statements) {
        resolve(stmt.get());
    }
}

void Resolver::resolve(Stmt* stmt) {
    stmt->accept(*this);
}

void Resolver::resolve(Expr* expr) {
    expr->accept(*this);
}

void Resolver::resolveLocal(const Expr& expr, const Token &identifier) {
//    for (int i = scopes.size() - 1; i >= 0; i--){
//        if (scopes[i].find(name.lexeme) != scopes[i].end()){
//            distances[expr] = scopes.size() - i - 1; //number of hops when resolving variable
//            return;
//        }
//    }

    //If it is not found we assume the variable was global


    if (scopes.empty())
        return;

    // Look for a variable starting from the innermost scope.
    int i = 0;
    for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope)
    {
        // If variable is found, then we resolve it.
        if (scope->find(identifier.lexeme) != scope->end())
        {
//                m_Interpreter.resolve(expr, scopes.size() - 1 - i);
//            distances[&expr] = scopes.size() - i - 1; //number of hops when resolving variable // remove this line and keep next ????
            m_Interpreter.resolve(const_cast<Expr *>(&expr), std::distance(scopes.rbegin(), scope));
            return;
        }
        i++;
    }
    // ... If never found, we can assume that the variable is global.
}

void Resolver::resolveFunction(Function& function, FunctionType type) {
    FunctionType enclosingFunction = currentFunction;
    currentFunction = type;

    beginScope();
    for (const Token& param : function.m_Params) {
        declare(param);
        define(param);
    }
    resolve(function.m_Body);
    endScope();
    currentFunction = enclosingFunction; // ???
}

void Resolver::resolveFunction(AnonFunction& function) {
    beginScope();
    for (const Token& param : function.m_Params) {
        declare(param);
        define(param);
    }
    resolve(function.m_Body);
    endScope();
}

void Resolver::declare(const Token& name) {
    if (scopes.empty()) return;

    // Get the innermost scope.
    std::unordered_map<std::string, bool> scope = scopes.back();

    // Don't allow the same variable declaration more than once.
    if (scope.find(name.lexeme) != scope.end()) {
        ErrorReporter::error(name.line, "Variable with this name already declared in this scope.");
        hadResolutionError = true;
    }


    // The m_Value associated with a key in the scope map represents whether we have completed
    // resolving the initializer for that variable.
    //scope.try_emplace(identifier.lexeme, false);

    scope[name.lexeme] = false;
    scopes.pop_back();
    scopes.emplace_back(scope);
}

void Resolver::define(const Token& name) {
    if (scopes.empty()) return;

    // Indicates that the variable has been fully initialized by setting the m_Value to true.
    //scopes.back()[identifier.lexeme] = true;

    // Get the innermost scope.
    std::unordered_map<std::string, bool> scope = scopes.back();

    scope[name.lexeme] = true;
    scopes.pop_back();
    scopes.emplace_back(scope);
}

void Resolver::beginScope() {
    scopes.push_back(std::unordered_map<std::string, bool>()); // change to emplace_back ???
    usages.push_back(std::unordered_map<std::string, int>()); // change to emplace_back ???
}

void Resolver::endScope() {
    std::unordered_map<std::string, int> last_element = usages.back();
    for (auto& pair : last_element) {
        if (pair.second == 0) {
            std::string warningMessage = "Variable " + pair.first + " was declared but never used.";
            ErrorReporter::warning(warningMessage.c_str());
        }
    }
    scopes.pop_back();
}

void Resolver::increaseUsage(const Token& name) {
    if (!scopes.empty() && (scopes.back().find(name.lexeme) != scopes.back().end())) {
        // increment usage count of the variable by one
        int count = 0;
        if (usages.back().count(name.lexeme) > 0) {
            count = usages.back().count(name.lexeme);
        }

        usages.back()[name.lexeme] = count + 1;
    }
}


// EXPRESSIONS

Object Resolver::visitSetExpr(Set& expr) {
    resolve(expr.m_Value.get());
    resolve(expr.m_Object.get());
    return Object::Null();
}

Object Resolver::visitLogicalExpr(Logical& expr) {
    resolve(expr.m_Left.get());
    resolve(expr.m_Right.get());
    return Object::Null();
}

Object Resolver::visitLiteralExpr(Literal& expr) {
    return Object::Null();
}

Object Resolver::visitGroupingExpr(Grouping& expr) {
    resolve(expr.m_Expression.get());
    return Object::Null();
}

Object Resolver::visitCallExpr(Call& expr) {
    resolve(expr.m_Callee.get());

    for (const auto& argument : expr.m_Arguments) {
        resolve(argument.get());
    }
    return Object::Null();
}

Object Resolver::visitAnonFunctionExpr(AnonFunction& expr) {
    resolveFunction(expr);
    return Object::Null();
}

Object Resolver::visitGetExpr(Get& expr) {
    resolve(expr.m_Object.get());
    return Object::Null();
}

Object Resolver::visitAssignExpr(Assign& expr) {
    resolve(expr.m_Value.get());
    resolveLocal(expr, expr.m_Name);

    increaseUsage(expr.m_Name);
    return Object::Null();
}

Object Resolver::visitBinaryExpr(Binary& expr) {
    resolve(expr.m_Left.get());
    resolve(expr.m_Right.get());
    return Object::Null();
}

Object Resolver::visitThisExpr(This& expr) {
    if (currentClass == CLASS_NONE) {
        ErrorReporter::error(expr.m_Keyword.line, "Cannot use 'this' outside of a class.");
        hadResolutionError = true;
        return Object::Null();
    }
    resolveLocal(expr, expr.m_Keyword);
    return Object::Null();
}

Object Resolver::visitSuperExpr(Super& expr) {
    if (currentClass == CLASS_NONE) {
        ErrorReporter::error(expr.m_Keyword.line, "Cannot use 'super' outside of a class.");
        hadResolutionError = true;
    } else if (currentClass != SUBCLASS) {
        ErrorReporter::error(expr.m_Keyword.line, "Cannot use 'super' in a class with no superclass.");
        hadResolutionError = true;
    }
    resolveLocal(expr, expr.m_Keyword);
    return Object::Null();
}

Object Resolver::visitUnaryExpr(Unary& expr) {
    resolve(expr.m_Right.get());
    return Object::Null();
}

Object Resolver::visitVariableExpr(Variable& expr) {
    if (!scopes.empty()) {
        auto last = scopes.back();
        auto searched = last.find(expr.m_VariableName.lexeme);
        if (searched != last.end() && !searched->second) {
            ErrorReporter::error(expr.m_VariableName.line, "Cannot read local variable in its own initializer.");
            hadResolutionError = true;
        }
    }

    resolveLocal(expr, expr.m_VariableName);
    increaseUsage(expr.m_VariableName);
    return Object::Null();
}

Object Resolver::visitTernaryExpr(Ternary& expr) {
    resolve(expr.m_Expr.get());
    resolve(expr.m_TrueExpr.get());
    resolve(expr.m_FalseExpr.get());
    return Object::Null();
}

// STATEMENTS

void Resolver::visitExpressionStmt(Expression& stmt) {
    resolve(stmt.m_Expression.get());
}

void Resolver::visitReturnStmt(Return& stmt) {
    if (currentFunction == FUNCTION_NONE) {
        ErrorReporter::error(stmt.m_Keyword.line, "Cannot return from top-level code.");
        hadResolutionError = true;
    }

    if (stmt.m_Value.has_value()) {
        if (currentFunction == INITIALIZER) {
            ErrorReporter::error(stmt.m_Keyword.line, "Cannot return a value from an initializer.");
            hadResolutionError = true;
        }
        resolve(stmt.m_Value->get());
    }
}

void Resolver::visitBreakStmt(Break& stmt) {
    // If not in a nested loop, add a new error.
    if (loopNestingLevel == 0) {
        ErrorReporter::error(stmt.m_Keyword, "Can't break outside of a loop.");
        hadResolutionError = true;
    }
    //throw BreakException(stmt.m_Keyword);
    //        throw BreakError(stmt.m_Keyword);
}

void Resolver::visitLetStmt(Let& stmt) {
    declare(stmt.m_Name);
    if (stmt.m_Initializer.has_value()) {
        resolve(stmt.m_Initializer->get());
    }
    define(stmt.m_Name);

    if (!usages.empty()) {
        std::unordered_map<std::string, int>& last_element = usages.back();
        last_element[stmt.m_Name.lexeme] = 0;
    }
}

void Resolver::visitWhileStmt(While& stmt) {
    loopNestingLevel++;
    resolve(stmt.m_Condition.get());
    resolve(stmt.m_Body.get());
}

void Resolver::visitIfStmt(If& stmt) {
    resolve(stmt.m_Condition.get());
    resolve(stmt.m_ThenBranch.get());
    if (stmt.m_ElseBranch.has_value()) {
        resolve(stmt.m_ElseBranch->get());
    }
}

void Resolver::visitBlockStmt(Block& stmt) {
    beginScope();
    resolve(stmt.m_Statements);
    endScope();
}

void Resolver::visitFunctionStmt(Function& stmt) {
    declare(stmt.m_Name);
    define(stmt.m_Name);
    resolveFunction(stmt, FUNCTION);
}

void Resolver::visitPrintStmt(Print& stmt) {
    if (stmt.m_Expression.has_value())
        resolve(stmt.m_Expression->get());
}

void Resolver::visitClazzStmt(Class& stmt) {
    ClassType enclosingClass = currentClass;
    currentClass = ClassType::CLASS;

    declare(stmt.m_Name);
    define(stmt.m_Name);

    if (stmt.m_Superclass.has_value() &&
        stmt.m_Name.lexeme == stmt.m_Superclass->get()->m_VariableName.lexeme) {
        ErrorReporter::error(stmt.m_Superclass->get()->m_VariableName.line, "A class cannot inherit from itself.");
        hadResolutionError = true;
    }

    if (stmt.m_Superclass.has_value()) {
        currentClass = ClassType::SUBCLASS;
        resolve(stmt.m_Superclass->get());
    }

    if (stmt.m_Superclass.has_value()) {
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
        FunctionType declaration = FunctionType::METHOD;
        if (method->m_Name.lexeme == "init") {
            declaration = FunctionType::INITIALIZER;
        }
        resolveFunction(*method, declaration);
    }

    endScope();

    if (stmt.m_Superclass.has_value()) endScope();

    currentClass = enclosingClass; // ????
}