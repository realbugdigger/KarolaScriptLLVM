#include "Interpreter.h"

#include "RuntimeError.h"

void Interpreter::interpret(std::vector<UniqueStmtPtr>& statements) {
    try {
        for (auto& statement : statements) {
            execute(statement.get());
        }
    } catch (RuntimeError& error) {
        lox::runtimeError(error);
    }
}

Object Interpreter::evaluate(Expr* expr) {
    return expr->accept(*this);
}

void Interpreter::execute(Stmt* stmt) {
    stmt->accept(*this);
}

