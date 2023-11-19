#pragma once

#include <any>
#include <iostream>
#include <memory>

#include "../parser/Stmt.h"
#include "../parser/Expr.h"
#include "Environment.h"
#include "KarolaScriptInstance.h"

class Interpreter : public StmtVisitor, public ExprVisitor<std::any> {
private:
    std::unique_ptr<Environment> globals = std::make_unique<Environment>();
    Environment* const global_environment;
    std::shared_ptr<Environment> environment;
    std::unordered_map<const Expr*, size_t> locals;

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
    void interpret(std::vector<std::shared_ptr<Stmt>>& statements) {
        try {
            for (auto& statement : statements) {
                execute(statement);
            }
        } catch (RuntimeError& error) {
            lox::runtimeError(error);
        }
    }

    std::any visitSetExpr(Set& expr) override {
        std::any object = evaluate(expr.m_Object);

        if (object.type() != typeid(KarolaScriptInstance)) {
            throw RuntimeError(expr.m_Name, "Only instances have fields.");
        }

        std::any value = evaluate(expr.m_Value);
        KarolaScriptInstance->set(expr.m_Value, value);
        return value;
    }

    std::any visitLogicalExpr(Logical& expr) override {
        // Evaluate the left operand of the logical expression.
        auto left = evaluate(expr.m_Left);

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
        return evaluate(expr.m_Right);
    }

    std::any visitLiteralExpr(Literal& expr) override {
        return expr.m_Literal;
    }

    std::any visitGroupingExpr(Grouping& expr) override {
        return evaluate(expr.m_Expression);
    }

    std::any visitCallExpr(Call& expr) override {
        // Evaluate the callee (the function or class being called).
        auto callee = evaluate(expr.m_Callee);

        // Collect the arguments passed to the function or class.
        std::vector<std::any> arguments;
        arguments.reserve(expr.m_Arguments.size());
        for (auto& arg : expr.m_Arguments)
        {
            arguments.emplace_back(evaluate(arg));
        }

        // Prevent calling objects which are not of callable type.
        std::unique_ptr<Callable> function;
        if (callee.type() == typeid(FunctionType))
        {
            function = std::make_unique<FunctionType>(std::any_cast<FunctionType>(callee));
        }
        else if (callee.type() == typeid(ClockCallable))
        {
            function = std::make_unique<ClockCallable>();
        }
        else if (callee.type() == typeid(PrintCallable))
        {
            function = std::make_unique<PrintCallable>(arguments.size());
        }
        else
        {
            // Throw an error if the callee is not callable (a function or class).
            throw RuntimeError(expr.m_Paren, expr.m_Paren.lexeme + " is not callable. Callable object must be a function or a class.");
        }

        // Check that the number of arguments passed to the function or class
        // matches the expected number
        if (arguments.size() != function->getArity())
        {
            throw RuntimeError(expr.m_Paren, "Expected " + std::to_string(function->getArity()) +
                                           " arguments but got " +
                                           std::to_string(arguments.size()) + " .");
        }

        // Return by calling the function.
        return function->call(*this, arguments);
    }

    std::any visitAnonFunctionExpr(AnonFunction& expr) override {
//        Stmt.Function stmt = new Stmt.Function(null, expr.params, expr.body);
//        LoxFunction function = new LoxFunction(stmt, environment);
        return nullptr;
    }

    std::any visitGetExpr(Get& expr) override {
        std::any object = evaluate(expr.m_Object);
        if (object.type() == typeid(KarolaScriptInstance)) {
            return (object.instance)->get(expr.m_Name);
        }

        throw RuntimeError(expr.m_Name, "Only instances have properties.");
    }

    std::any visitAssignExpr(Assign& expr) override {
        std::any value = evaluate(expr.m_Value);

        // environment->assign(expr->m_Name, m_Value);
        auto distance = locals.find(expr);
        if (distance != locals.end()) {
            environment->assignAt(distance->second, expr.m_Name, value);
        } else {
            globals->assign(expr.m_Name, value);
        }
        return value;
    }

    std::any visitBinaryExpr(Binary& expr) override {
        // Evaluate the left-hand side and right-hand side operands of the binary expression
        auto left = evaluate(expr.m_Left);
        auto right = evaluate(expr.m_Right);

        // Dereference the pointer in case evaluate returns one
        if (left.type() == typeid(std::shared_ptr<std::any>))
        {
            left = *(std::any_cast<std::shared_ptr<std::any>>(left));
        }

        if (right.type() == typeid(std::shared_ptr<std::any>))
        {
            right = *(std::any_cast<std::shared_ptr<std::any>>(right));
        }

        // Check the type of the operator.
        switch (expr.m_Operator.type)
        {
            case TOKEN_MINUS:
                checkNumberOperands(expr.m_Operator, left, right);
                return std::any_cast<double>(left) - std::any_cast<double>(right);

            case TOKEN_SLASH:
                checkNumberOperands(expr.m_Operator, left, right);

                // Throw error if right operand is 0.
                if (std::any_cast<double>(right) == 0)
                {
                    throw RuntimeError(expr.m_Operator, "Division by 0.");
                }
                return std::any_cast<double>(left) / std::any_cast<double>(right);

            case TOKEN_STAR:
                checkNumberOperands(expr.m_Operator, left, right);
                return std::any_cast<double>(left) * std::any_cast<double>(right);

            case TOKEN_GREATER:
                checkNumberOperands(expr.m_Operator, left, right);
                return std::any_cast<double>(left) > std::any_cast<double>(right);

            case TOKEN_GREATER_EQUAL:
                checkNumberOperands(expr.m_Operator, left, right);
                return std::any_cast<double>(left) >= std::any_cast<double>(right);

            case TOKEN_LESS:
                checkNumberOperands(expr.m_Operator, left, right);
                return std::any_cast<double>(left) < std::any_cast<double>(right);

            case TOKEN_LESS_EQUAL:
                checkNumberOperands(expr.m_Operator, left, right);
                return std::any_cast<double>(left) <= std::any_cast<double>(right);

            case TOKEN_EQUAL_EQUAL:
                return isEqual(left, right);

            case TOKEN_BANG_EQUAL:
                return !isEqual(left, right);

            case TOKEN_PLUS:
                if (left.type() == typeid(std::string) && right.type() == typeid(std::string))
                {
                    return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
                }
                else if (left.type() == typeid(double) && right.type() == typeid(double))
                {
                    return std::any_cast<double>(left) + std::any_cast<double>(right);
                }
                else if (left.type() == typeid(double) && right.type() == typeid(std::string))
                {
                    // Remove trailing zeroes.
                    std::string num_as_string = std::to_string(std::any_cast<double>(left));
                    num_as_string.erase(num_as_string.find_last_not_of('0') + 1, std::string::npos);
                    num_as_string.erase(num_as_string.find_last_not_of('.') + 1, std::string::npos);
                    return num_as_string + std::any_cast<std::string>(right);
                }
                else if (left.type() == typeid(std::string) && right.type() == typeid(double))
                {
                    // Remove trailing zeroes.
                    std::string num_as_string = std::to_string(std::any_cast<double>(right));
                    num_as_string.erase(num_as_string.find_last_not_of('0') + 1, std::string::npos);
                    num_as_string.erase(num_as_string.find_last_not_of('.') + 1, std::string::npos);
                    return std::any_cast<std::string>(left) + num_as_string;
                }

                throw RuntimeError(expr.m_Operator, "Operands must be of type string or number.");

            default:
                return {};
        }
    }

    std::any visitThisExpr(This& expr) override {
        return lookUpVariable(expr.m_Keyword, expr);
    }

    std::any visitSuperExpr(Super& expr) override {
        int distance = locals[expr];
        KSClass superclass = (KSClass) environment->getAt(distance, "super");

        // "this" is always one level nearer than "super"'s environment.
        KSInstance instance = environment->getAt(distance - 1, "this");

        std::shared_ptr<KSFunction> method = superclass.ks_class->findMethod(expr.m_Method.lexeme);

        if (method == nullptr) {
            throw RuntimeError(expr.m_Method, "Undefined property '" + expr.m_Method.lexeme + "'.");
        }

        std::shared_ptr<KSFunction> binded_method =  method->bind(instance.instance);
        return Object::make_fun_obj(binded_method);
    }

    std::any visitUnaryExpr(Unary& expr) override {
        // Evaluate the right-hand side operand of the unary expression.
        auto right = evaluate(expr.m_Right);

        // Dereference pointer in case evaluate returns one
        if (right.type() == typeid(std::shared_ptr<std::any>)) {
            right = *(std::any_cast<std::shared_ptr<std::any>>(right));
        }

        // Check the type of the operator
        switch (expr.m_Operator.type)
        {
            case TokenType::TOKEN_MINUS:
                // Ensure that the right-hand side operand is a number.
                checkNumberOperand(expr.m_Operator, right);
                // Return the negation of the right-hand side operand.
                return -(std::any_cast<double>(right));

            case TokenType::TOKEN_BANG:
                // Return the negation of the truthiness of the right-hand side operand.
                return !isTruthy(right);

            default:
                return {}; // Unreachable.
        }
    }

    std::any visitVariableExpr(Variable& expr) override {
        return lookUpVariable(expr.m_VariableName, expr);
    }

    std::any visitTernaryExpr(Ternary& expr) override {
        std::any right = evaluate(expr.m_FalseExpr);
        std::any left = evaluate(expr.m_TrueExpr);
        std::any truthyExpr = evaluate(expr.m_Expr);

        if (isTruthy(truthyExpr))
            return left;
        return right;
    }

    void visitExpressionStmt(Expression& stmt) override {
        evaluate(stmt.m_Expression);
    }

    void visitReturnStmt(Return& stmt) override {
        std::any value;
        // If the return statement is not void, evaluate the expression.
        if (stmt.m_Value != nullptr) {
            value = evaluate(stmt.m_Value);
        }

        throw ReturnException(value);
    }

    void visitBreakStmt(Break& stmt) override {
        throw BreakException(stmt.m_Keyword);
//        throw BreakError(stmt.m_Keyword);
    }

    void visitLetStmt(Let& stmt) override {
        // maybe check if already contains key with stmt.m_Name.lexeme, if yes throw RuntimeError

        std::any value;
        // If the variable has an initializer, evaluate the initializer.
        if (stmt.m_Initializer != nullptr) {
            value = evaluate(stmt.m_Initializer);
        }

        // Define the variable in the current environment with the given identifier and value
        environment->define(stmt.m_Name.lexeme, value);
    }

    void visitWhileStmt(While& stmt) override {
        try {
            while (isTruthy(evaluate(stmt.m_Condition))) {
                std::any obj = evaluate(stmt.m_Condition);
                execute(stmt.m_Body);
            }
        }catch (BreakException& e) {
            // catching break carefully and exiting loop
        }
    }

    void visitIfStmt(If& stmt) override {
        try {
            if (isTruthy(evaluate(stmt.m_Condition))) {
                execute(stmt.m_ThenBranch);
            } else if (stmt.m_ElseBranch != nullptr) {
                execute(stmt.m_ElseBranch);
            }
        } catch (BreakException& e) {
            // catching break carefully and exiting loop
        }
    }

    void visitBlockStmt(Block& stmt) override {
        executeBlock(stmt.m_Statements, std::make_shared<Environment>(environment));
    }

    void visitFunctionStmt(Function& stmt) override {
        std::shared_ptr<KarolaScriptFunction> function(new KarolaScriptFunction(stmt, environment, false));
        Object obj = Object::make_fun_obj(function);
        environment->define(stmt.m_Name.lexeme, obj);
    }

    void visitPrintStmt(Print& stmt) override {
        std::any value = evaluate(stmt.m_Expression);
        std::cout << stringify(value) << std::endl;
    }

    void visitClazzStmt(Class& stmt) override {
        Object superclass = Object::make_nil_obj();
        if (stmt.m_Superclass != nullptr) {
            superclass = evaluate(stmt.m_Superclass);
            if (superclass.type != Object::Object_class) {
                throw RuntimeError(stmt.m_Superclass->name, "Superclass must be a class.");
            }
        }

        environment->define(stmt.m_Name.lexeme, nullptr);

        if (stmt.m_Superclass != nullptr) {
            environment = std::make_shared<Environment>(environment);
            environment->define("super", superclass);
        }

        map<string, shared_ptr<KarolaScriptFunction>> methods;
        for (auto method : stmt.m_Methods) {
            bool is_init = method->m_Name.lexeme == "init";
            auto function = std::make_shared<KarolaScriptFunction>(method, environment, is_init);

            methods[method->m_Name.lexeme] = function;
        }

        auto klass = std::make_shared<KarolaScriptClass>(stmt.m_Name.lexeme, superclass.lox_class, methods);

        if (superclass.type != Object::Object_nil) {
            environment = environment->enclosing;
        }

        environment->assign(stmt.m_Name, Object::make_class_obj(klass));
    }

    void executeBlock(std::vector<std::shared_ptr<Stmt>>& statements, std::shared_ptr<Environment> enclosing_env) {
        // Enter a new environment.
        EnvironmentGuard environment_guard{*this, std::move(enclosing_env)};
        for (auto& statement : statements) {
            assert(statement != nullptr);
            execute(statement);
        }
    }

public:
    std::any evaluate(std::shared_ptr<Expr>& expr) {
        return expr->accept(*this);
    }

    void execute(std::shared_ptr<Stmt>& stmt) {
        stmt->accept(*this);
    }

    // KarolaScript follows Ruby’s simple rule: `false` and `null` are falsey, and everything else is truthy
    bool isTruthy(const std::any& object) const {
        // This case would indicate a null m_Value.
        if (!object.has_value()) {
            return false;
        }

        // If the object is of type bool, return the cast m_Value.
        if (object.type() == typeid(bool)) {
            return std::any_cast<bool>(object);
        }

        // Object has m_Value and is not a false boolean, Therefore it is considered truthy.
        return true;
    }

    bool isEqual(const std::any& lhs, const std::any& rhs) const {
        if (!lhs.has_value() && !rhs.has_value())
        {
            return true;
        }
        if (!lhs.has_value())
        {
            return false;
        }

        if (lhs.type() != rhs.type())
        {
            return false;
        }

        if (lhs.type() == typeid(bool))
        {
            return std::any_cast<bool>(lhs) == std::any_cast<bool>(rhs);
        }

        if (lhs.type() == typeid(double))
        {
            return std::any_cast<double>(lhs) == std::any_cast<double>(rhs);
        }

        if (lhs.type() == typeid(std::string))
        {
            return std::any_cast<std::string>(lhs) == std::any_cast<std::string>(rhs);
        }

        // If the type is not bool, double, or std::string, return false
        return false;
    }

    void checkNumberOperand(const Token& op, const std::any& operand) const
    {
        if (operand.type() != typeid(double))
        {
            throw RuntimeError(op, "Operand must be a number.");
        }
    }

    void checkNumberOperands(const Token& op, const std::any& lhs, const std::any& rhs) const
    {
        // Throws a runtime error if either the left-hand side or the right-hand side operand is not of
        // type double.
        if (lhs.type() != typeid(double) || rhs.type() != typeid(double))
        {
            throw RuntimeError(op, "Operands must be numbers.");
        }
    }

    std::string stringify(const std::any& object) {
        if (object.type() == typeid(std::nullptr_t))
            return "null";

        auto text = std::any_cast<std::string>(object);
        std::string check_str = ".000000";
        if (endsWith(text, check_str)) {
            return text.erase(text.size() - check_str.size());
        }
        return text;
    }

    bool endsWith(const std::string& str, const std::string& suffix) {
        if (str.length() < suffix.length()) {
            return false;
        }
        return str.substr(str.length() - suffix.length()) == suffix;
    }

    void resolve(const std::shared_ptr<Expr>& expr, int depth) {
        //locals[expr] = depth;
        locals.try_emplace(*expr, depth);
    }
};
