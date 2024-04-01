#include "CodeGenVisitor.h"

#include "../interpreter/RuntimeError.h"

CodeGenVisitor::CodeGenVisitor(Interpreter& interpreter) : m_Interpreter(interpreter) {}

void CodeGenVisitor::generate(std::vector<UniqueStmtPtr> &statements) {
    for (auto& statement : statements) {
        compile(statement.get());
    }
}

llvm::Value* CodeGenVisitor::visitBinaryExpr(Binary &expr) {
    Object leftObject = m_Interpreter.evaluate(expr.m_Left.get());
    Object rightObject = m_Interpreter.evaluate(expr.m_Left.get());

    llvm::Value* left = gen(leftObject);
    llvm::Value* right = gen(rightObject);

    switch (expr.m_Operator.type) {
        case TOKEN_MINUS:
            m_Interpreter.checkNumberOperands(expr.m_Operator, leftObject, rightObject);
            return builder->CreateSub(left, right);
        case TOKEN_SLASH:
            m_Interpreter.checkNumberOperands(expr.m_Operator, leftObject, rightObject);

            // Throw error if right operand is 0.
            if (rightObject.getNumber() == 0) {
                throw RuntimeError(expr.m_Operator, "Division by 0.");
            }
            return builder->CreateSDiv(left, right);

        case TOKEN_STAR:
            m_Interpreter.checkNumberOperands(expr.m_Operator, leftObject, rightObject);
            return builder->CreateMul(left, right);

        case TOKEN_GREATER:
            m_Interpreter.checkNumberOperands(expr.m_Operator, leftObject, rightObject);
            return builder->CreateICmpUGT(left, right);

        case TOKEN_GREATER_EQUAL:
            m_Interpreter.checkNumberOperands(expr.m_Operator, leftObject, rightObject);
            return builder->CreateICmpUGE(left, right);

        case TOKEN_LESS:
            m_Interpreter.checkNumberOperands(expr.m_Operator, leftObject, rightObject);
            return builder->CreateICmpULT(left, right);

        case TOKEN_LESS_EQUAL:
            m_Interpreter.checkNumberOperands(expr.m_Operator, leftObject, rightObject);
            return builder->CreateICmpULE(left, right);

        case TOKEN_EQUAL_EQUAL:
            return builder->CreateICmpEQ(left, right);

        case TOKEN_BANG_EQUAL:
            return builder->CreateICmpNE(left, right);

        case TOKEN_PLUS:
            if (leftObject.isString() && rightObject.isString()) {
                return builder->CreateGlobalString(leftObject.getString() + rightObject.getString());
            }
            else if (leftObject.isNumber() && rightObject.isNumber()) {
                return builder->CreateAdd(left, right);
            }
            else if (leftObject.isNumber() && rightObject.isString()) {
                // Remove trailing zeroes.
                std::string num_as_string = std::to_string(leftObject.getNumber());
                num_as_string.erase(num_as_string.find_last_not_of('0') + 1, std::string::npos);
                num_as_string.erase(num_as_string.find_last_not_of('.') + 1, std::string::npos);
                return builder->CreateGlobalString(num_as_string + rightObject.getString());
            }
            else if (leftObject.isString() && rightObject.isNumber()) {
                // Remove trailing zeroes.
                std::string num_as_string = std::to_string(rightObject.getNumber());
                num_as_string.erase(num_as_string.find_last_not_of('0') + 1, std::string::npos);
                num_as_string.erase(num_as_string.find_last_not_of('.') + 1, std::string::npos);
                return builder->CreateGlobalString(leftObject.getString() + num_as_string);
            }

            throw RuntimeError(expr.m_Operator, "Operands must be of type string or number.");

        default:
            return {};
    }
}

void CodeGenVisitor::visitFunctionStmt(Function &stmt) {
    m_Generator.createFunction(stmt.m_Name, llvm::FunctionType::get(/* return type*/ builder->getInt32Ty(),true))
}

void CodeGenVisitor::compile(Stmt *stmt) {
    stmt->accept(*this);
}

llvm::Value *CodeGenVisitor::gen(Object object) {
    if (object.isNumber())
        return builder->getInt32(object.getNumber());
    if (object.isString())
        return builder->CreateGlobalString(object.getString());
    if (object.isBoolean())
        return builder->getInt1(object.getBoolean());
}