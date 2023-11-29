#include "Interpreter.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>

#include "KarolaScriptClass.h"
#include "RuntimeError.h"
#include "../util/ErrorReporter.h"
#include "KarolaScriptFunction.h"
#include "KarolaScriptCallable.h"
#include "../util/Utils.h"
#include "ks_stdlib/StdLibFunctions.h"
#include "KarolaScriptAnonFunction.h"

Interpreter::Interpreter() {
    globals = std::make_unique<Environment>();
    environment = globals;
    loadNativeFunctions();
}

void Interpreter::interpret(std::vector<UniqueStmtPtr>& statements) {
    try {
        for (auto& statement : statements) {
            execute(statement.get());
        }
    } catch (RuntimeError& error) {
        ErrorReporter::runtimeError(error);
    }
}

void Interpreter::resolve(Expr* expr, int depth) {
    localsDistances[expr] = depth;
}

Object Interpreter::evaluate(Expr* expr) {
    return expr->accept(*this);
}

void Interpreter::execute(Stmt* stmt) {
    stmt->accept(*this);
}

void Interpreter::executeBlock(const std::vector<UniqueStmtPtr>& statements, std::shared_ptr<Environment> enclosing_env) {
    // Enter a new environment.
    EnvironmentGuard environment_guard{*this, std::move(enclosing_env)};
    for (auto& statement : statements) {
        execute(statement.get());
    }
}

bool Interpreter::isTruthy(const Object& object) const {
    if (object.isNull()) {
        return false;
    }

    // If the object is of type bool, return the cast value.
    if (object.isBoolean()) {
        return object.getBoolean();
    }

    // Object has value and is not a false boolean, Therefore it is considered truthy.
    return true;
}

bool Interpreter::isEqual(const Object& lhs, const Object& rhs) const {
    if (lhs.isNull() && rhs.isNull()) {
        return true;
    }
    if (lhs.isNull()) {
        return false;
    }

    if (lhs.type != rhs.type) {
        return false;
    }

    if (lhs.isBoolean()) {
        return lhs.getBoolean() == rhs.getBoolean();
    }

    if (lhs.isNumber()) {
        return lhs.getNumber() == rhs.getNumber();
    }

    if (lhs.isString()) {
        return lhs.getString() == rhs.getString();
    }

    // If the type is not bool, double, or std::string, return false
    return false;
}

void Interpreter::checkNumberOperand(const Token& op, const Object& operand) const {
    if (!operand.isNumber()) {
        throw RuntimeError(op, "Operand must be a number.");
    }
}

void Interpreter::checkNumberOperands(const Token& op, const Object& lhs, const Object& rhs) const {
    // Throws a runtime error if either the left-hand side or the right-hand side operand is not of
    // type double.
    if (!lhs.isNumber() || !rhs.isNumber()) {
        throw RuntimeError(op, "Operands must be numbers.");
    }
}

std::string Interpreter::stringify(const Object& object) {
    if (object.isNull())
        return "null";

    switch (object.type) {
        case ObjType::OBJTYPE_NULL:
            return "null";
        case ObjType::OBJTYPE_BOOL:
            return (object.getBoolean() ? std::string("true") : std::string("false"));
        case ObjType::OBJTYPE_NUMBER:
            if (std::abs(floor(object.getNumber())) == std::abs(object.getNumber())){ //If it has no decimal part
                return std::to_string((long long) object.getNumber());
            } else {
                return std::to_string(object.getNumber());
            }
        case ObjType::OBJTYPE_STRING:
        {
            std::string s = object.getString();
            utils::replaceAll(s, "\\n", "\n");
            utils::replaceAll(s, "\\t", "\t");
            return s;
        }
        case ObjType::OBJTYPE_CALLABLE:
            return object.getCallable()->toString();
        case ObjType::OBJTYPE_INSTANCE:
            return object.getClassInstance()->toString();
        default:
            throw std::runtime_error("Object has no string representation");
    }
}

void Interpreter::loadNativeFunctions() {
    SharedCallablePtr clock = std::make_shared<stdlibFunctions::Clock>();
    SharedCallablePtr sleep = std::make_shared<stdlibFunctions::Sleep>();

    std::vector<Object> functions = {Object(clock), Object(sleep)};
    for (const auto &function : functions) {
        globals->define(function.getCallable()->name(), function);
    }
}

Object Interpreter::lookupVariable(const Token& identifier, const Expr* variableExpr) {
    if (localsDistances.find(variableExpr) != localsDistances.end()){
        return environment->getAt(localsDistances[variableExpr], identifier.lexeme);
    }
    return globals->lookup(identifier);
}

// EXPRESSIONS

Object Interpreter::visitSetExpr(Set& expr) {
    Object object = evaluate(expr.m_Object.get());

    if (!object.isInstance()) {
        throw RuntimeError(expr.m_Name, "Only instances have fields.");
    }

    Object value = evaluate(expr.m_Value.get());
    object.getClassInstance()->setProperty(expr.m_Name, value);
    return value;
}

Object Interpreter::visitLogicalExpr(Logical& expr) {
    // Evaluate the left operand of the logical expression.
    auto left = evaluate(expr.m_Left.get());

    // If the operator is 'OR' and the left operand is truthy, return the left operand.
    if (expr.m_Operator.type == TokenType::TOKEN_OR) {
        if (isTruthy(left)) {
            return left;
        }
    }

    // If the operator is 'AND' and the left operand is falsy, return the left operand.
    else if (!isTruthy(left)) {
        return left;
    }

    // If the left operand didn't short-circuit the evaluation, evaluate the right operand and
    // return it.
    return evaluate(expr.m_Right.get());
}

Object Interpreter::visitLiteralExpr(Literal& expr) {
    return expr.m_Literal;
}

Object Interpreter::visitGroupingExpr(Grouping& expr) {
    return evaluate(expr.m_Expression.get());
}

Object Interpreter::visitCallExpr(Call& callExpr) {
    Object callee = evaluate(callExpr.m_Callee.get());

    std::vector<Object> arguments;
    for (const UniqueExprPtr &arg : callExpr.m_Arguments) {
        Object argObject = evaluate(arg.get());
        if (argObject.isAnonFunction()) {
            KarolaScriptFunction* ksFunction = dynamic_cast<KarolaScriptFunction *>(callee.getCallable().get());
            environment->define(ksFunction->m_Declaration->m_Name.lexeme, argObject);
            arguments.push_back(argObject);
        } else {
            arguments.push_back(argObject);
        }
    }

    if (!callee.isCallable() && !callee.isAnonFunction()) {
        throw RuntimeError("Expression is not callable", callExpr.m_Paren.line);
    }
    KarolaScriptCallable* callable = callee.getCallable().get();
    if (arguments.size() != callable->arity()) {
        std::stringstream ss;
        ss  << callable->name() << " expected " << callable->arity() << " argument(s) but instead got " << arguments.size();
        throw RuntimeError(ss.str(), callExpr.m_Paren.line);
    }

    return callable->call(*this, arguments);
}

Object Interpreter::visitAnonFunctionExpr(AnonFunction& expr) {
    SharedCallablePtr anonFunction = std::make_shared<KarolaScriptAnonFunction>(&expr, environment);
    Object anonFunctionObject(anonFunction);
    return anonFunctionObject;
}

Object Interpreter::visitGetExpr(Get& expr) {
    Object object = evaluate(expr.m_Object.get());

    // lookup static methods within the class first before looking at instance methods
    if (object.isCallable() && object.getCallable()->m_Type == KarolaScriptCallable::CLASS) {
        KarolaScriptClass* clazz = dynamic_cast<KarolaScriptClass*>(object.getCallable().get());
        return clazz->getProperty(expr.m_Name);
    }
    if (object.isInstance()) {
        return object.getClassInstance()->getProperty(expr.m_Name);
    }

    throw RuntimeError(expr.m_Name, "Only instances have properties.");
}

Object Interpreter::visitAssignExpr(Assign& expr) {
    Object value = evaluate(expr.m_Value.get());

    auto distance = localsDistances.find(&expr);
    if (distance != localsDistances.end()) {
        environment->assignAt(distance->second, expr.m_Name, value);
    } else {
        globals->assign(expr.m_Name, value);
    }
    return value;
}

Object Interpreter::visitBinaryExpr(Binary& expr) {
    // Evaluate the left-hand side and right-hand side operands of the binary expression
    Object left = evaluate(expr.m_Left.get());
    Object right = evaluate(expr.m_Right.get());

    // Check the type of the operator.
    switch (expr.m_Operator.type) {
        case TOKEN_MINUS:
            checkNumberOperands(expr.m_Operator, left, right);
            return Object(left.getNumber() - right.getNumber());

        case TOKEN_SLASH:
            checkNumberOperands(expr.m_Operator, left, right);

            // Throw error if right operand is 0.
            if (right.getNumber() == 0) {
                throw RuntimeError(expr.m_Operator, "Division by 0.");
            }
            return Object(left.getNumber() / right.getNumber());;

        case TOKEN_STAR:
            checkNumberOperands(expr.m_Operator, left, right);
            return Object(left.getNumber() * right.getNumber());

        case TOKEN_GREATER:
            checkNumberOperands(expr.m_Operator, left, right);
            return Object(left.getNumber() > right.getNumber());

        case TOKEN_GREATER_EQUAL:
            checkNumberOperands(expr.m_Operator, left, right);
            return Object(left.getNumber() >= right.getNumber());

        case TOKEN_LESS:
            checkNumberOperands(expr.m_Operator, left, right);
            return Object(left.getNumber() < right.getNumber());

        case TOKEN_LESS_EQUAL:
            checkNumberOperands(expr.m_Operator, left, right);
            return Object(left.getNumber() <= right.getNumber());

        case TOKEN_EQUAL_EQUAL:
            return Object(isEqual(left, right));

        case TOKEN_BANG_EQUAL:
            return Object(!isEqual(left, right));

        case TOKEN_PLUS:
            if (left.isString() && right.isString()) {
                return Object(left.getString() + right.getString());
            }
            else if (left.isNumber() && right.isNumber()) {
                return Object(left.getNumber() + right.getNumber());
            }
            else if (left.isNumber() && right.isString()) {
                // Remove trailing zeroes.
                std::string num_as_string = std::to_string(left.getNumber());
                num_as_string.erase(num_as_string.find_last_not_of('0') + 1, std::string::npos);
                num_as_string.erase(num_as_string.find_last_not_of('.') + 1, std::string::npos);
                return Object(num_as_string + right.getString());
            }
            else if (left.isString() && right.isNumber()) {
                // Remove trailing zeroes.
                std::string num_as_string = std::to_string(right.getNumber());
                num_as_string.erase(num_as_string.find_last_not_of('0') + 1, std::string::npos);
                num_as_string.erase(num_as_string.find_last_not_of('.') + 1, std::string::npos);
                return Object(left.getString() + num_as_string);
            }

            throw RuntimeError(expr.m_Operator, "Operands must be of type string or number.");

        default:
            return {};
    }
}

Object Interpreter::visitThisExpr(This& expr) {
    return lookupVariable(expr.m_Keyword, &expr);
}

Object Interpreter::visitSuperExpr(Super& expr) {
//    int distance = localsDistances[&expr]; // distance from current env to env where the superclass is stored
    // Get the superclass object and cast it to KarolaScriptClass
//    Object superclassObject = environment->getAt(distance, "super");
    Object superclassObject = environment->lookup("super");
    KarolaScriptClass* superclass = dynamic_cast<KarolaScriptClass*>(superclassObject.getCallable().get());

    // "this" is always one level nearer than "super"'s environment.
    Object instanceObject = environment->lookup("this");

    std::optional<Object> methodObj = superclass->findMethod(expr.m_Method.lexeme);
    if (!methodObj.has_value()){
        throw RuntimeError("Undefined property '" + expr.m_Method.lexeme + "'.", expr.m_Keyword.line);
    }
    KarolaScriptFunction* method = dynamic_cast<KarolaScriptFunction*>(methodObj.value().getCallable().get());

    //Bind "this" to the superclass' method. Even though the method comes from the superclass, "this" refers to the instance that is
    //calling the method.
    SharedCallablePtr bindedMethod(method->bind(instanceObject.getClassInstance()));
    Object bindedMethodObj(bindedMethod);
    return bindedMethodObj;
}

Object Interpreter::visitUnaryExpr(Unary& expr) {
    // Evaluate the right-hand side operand of the unary expression.
    Object right = evaluate(expr.m_Right.get());

    // Check the type of the operator
    switch (expr.m_Operator.type)
    {
        case TokenType::TOKEN_MINUS:
            // Ensure that the right-hand side operand is a number.
            checkNumberOperand(expr.m_Operator, right);
            // Return the negation of the right-hand side operand.
            return Object(-right.getNumber());

        case TokenType::TOKEN_BANG:
            // Return the negation of the truthiness of the right-hand side operand.
            return Object(!isTruthy(right));

        default:
            return Object::Null(); // Unreachable.
    }
}

Object Interpreter::visitVariableExpr(Variable& expr) {
    return lookupVariable(expr.m_VariableName, &expr);
}

Object Interpreter::visitTernaryExpr(Ternary& expr) {
    Object right = evaluate(expr.m_FalseExpr.get());
    Object left = evaluate(expr.m_TrueExpr.get());
    Object truthyExpr = evaluate(expr.m_Expr.get());

    if (isTruthy(truthyExpr))
        return left;
    return right;
}

void Interpreter::visitExpressionStmt(Expression& stmt) {
    evaluate(stmt.m_Expression.get());
}

void Interpreter::visitReturnStmt(Return& stmt) {
    Object value;
    // If the return statement is not void, evaluate the expression.
    if (stmt.m_Value.has_value()) {
        value = evaluate(stmt.m_Value->get());
}

throw ReturnException(value);
}

void Interpreter::visitBreakStmt(Break& stmt) {
    throw BreakException(stmt.m_Keyword);
}

void Interpreter::visitLetStmt(Let& stmt) {
    // maybe check if already contains key with stmt.m_Name.lexeme, if yes throw RuntimeError

    Object value;
    // If the variable has an initializer, evaluate the initializer.
    if (stmt.m_Initializer.has_value()) {
        value = evaluate(stmt.m_Initializer->get());
    }

    // Define the variable in the current environment with the given identifier and value
    environment->define(stmt.m_Name.lexeme, value);
}

void Interpreter::visitWhileStmt(While& stmt) {
    try {
        while (isTruthy(evaluate(stmt.m_Condition.get()))) {
            execute(stmt.m_Body.get());
        }
    } catch (BreakException& e) {
        // catching break carefully and exiting loop
    }
}

void Interpreter::visitIfStmt(If& stmt) {
    try {
        if (isTruthy(evaluate(stmt.m_Condition.get()))) {
            execute(stmt.m_ThenBranch.get());
        } else if (stmt.m_ElseBranch.has_value()) {
            execute(stmt.m_ElseBranch->get());
        }
    } catch (BreakException& e) {
        // catching break carefully and exiting loop
    }
}

void Interpreter::visitBlockStmt(Block& stmt) {
    executeBlock(stmt.m_Statements, std::make_shared<Environment>(environment));
}

void Interpreter::visitFunctionStmt(Function& stmt) {
    SharedCallablePtr function = std::make_shared<KarolaScriptFunction>(&stmt, environment, false);
    Object functionObject(function);
    environment->define(stmt.m_Name.lexeme, functionObject);
}

void Interpreter::visitPrintStmt(Print& printStmt) {
    if (!printStmt.m_Expression.has_value()){
        std::cout << "\n";
        return;
    }

    Object value = evaluate(printStmt.m_Expression->get());
    std::cout << stringify(value) << std::endl;
}

void Interpreter::visitClazzStmt(Class& clazzStmt) {
    environment->define(clazzStmt.m_Name.lexeme, Object::Null());

    Object superclass = Object::Null();
    if (clazzStmt.m_Superclass.has_value()) {
        superclass = evaluate(clazzStmt.m_Superclass.value().get());
        if (!superclass.isCallable() || superclass.getCallable()->m_Type != KarolaScriptCallable::CallableType::CLASS) {
            throw RuntimeError(clazzStmt.m_Superclass->get()->m_VariableName, "Superclass must be a class.");
        }
    }

    std::optional<SharedCallablePtr> superclassPtr = std::nullopt;
    if (clazzStmt.m_Superclass.has_value()) {
        superclassPtr = superclass.getCallable();
        // create a new environment that binds "super" to the superclass
        environment = std::make_shared<Environment>(environment);
        environment->define("super", superclass);
    }

    std::unordered_map<std::string, Object> methods;
    for (const auto& method : clazzStmt.m_Methods) {
        bool is_init = method->m_Name.lexeme == "init";
        SharedCallablePtr callable = std::make_shared<KarolaScriptFunction>(method.get(), environment, is_init);
        Object functionObject(callable);
        methods[method->m_Name.lexeme] = functionObject;
    }

    std::unordered_map<std::string, Object> staticMethods;
    for (const auto& staticMethod : clazzStmt.m_StaticMethods) {
        SharedCallablePtr callable = std::make_shared<KarolaScriptFunction>(staticMethod.get(), environment, false);
        Object staticFunctionObject(callable);
        staticMethods[staticMethod->m_Name.lexeme] = staticFunctionObject;
    }

    if (!superclass.isNull()) {
        //pop latest environment
        environment = environment->m_Enclosing;
    }

    SharedCallablePtr klass(KarolaScriptMetaClass::createClass(clazzStmt.m_Name.lexeme, superclassPtr, methods, staticMethods));
    Object classObject(klass);
    environment->assign(clazzStmt.m_Name, classObject);
}