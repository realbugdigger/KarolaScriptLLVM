#pragma once

#include <memory>
#include <utility>
#include <vector>
#include <any>

#include "../lexer/Token.h"
//#include "Stmt.h"

class Stmt;

class Assign;
class Binary;
class Call;
class AnonFunction;
class Get;
class Grouping;
class Literal;
class Logical;
class Set;
class Super;
class This;
class Unary;
class Ternary;
class Variable;

template<typename R>
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual R visitAssignExpr(Assign& expr) = 0; //
    virtual R visitBinaryExpr(Binary& expr) = 0; //
    virtual R visitCallExpr(Call& expr) = 0; //
    virtual R visitAnonFunctionExpr(AnonFunction& expr) = 0; //
    virtual R visitGetExpr(Get& expr) = 0; //
    virtual R visitGroupingExpr(Grouping& expr) = 0; //
    virtual R visitLiteralExpr(Literal& expr) = 0; //
    virtual R visitLogicalExpr(Logical& expr) = 0; //
    virtual R visitSetExpr(Set& expr) = 0; //
    virtual R visitSuperExpr(Super& expr) = 0; //
    virtual R visitThisExpr(This& expr) = 0; //
    virtual R visitUnaryExpr(Unary& expr) = 0; //
    virtual R visitTernaryExpr(Ternary& expr) = 0; //
    virtual R visitVariableExpr(Variable& expr) = 0; //
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor<std::any>& visitor) = 0;
};

class Assign : public Expr {
public:
    Token m_Name;
    std::shared_ptr<Expr> m_Value;

    Assign(Token& name, std::shared_ptr<Expr>& value)
            : m_Name(std::move(name)), m_Value(std::move(value)) {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitAssignExpr(*this);
    }
};

class Binary : public Expr {
public:
    std::shared_ptr<Expr> m_Left;
    Token m_Operator;
    std::shared_ptr<Expr> m_Right;

    Binary(std::shared_ptr<Expr>& left, Token& operator_, std::shared_ptr<Expr>& right)
            : m_Left(std::move(left)), m_Operator(std::move(operator_)), m_Right(std::move(right)) {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitBinaryExpr(*this);
    }
};

class Call : public Expr {
public:
    std::shared_ptr<Expr> m_Callee;
    Token m_Paren;
    std::vector<std::shared_ptr<Expr>> m_Arguments;

    Call(std::shared_ptr<Expr>& callee, Token& paren, std::vector<std::shared_ptr<Expr>>& arguments)
            : m_Callee(std::move(callee)), m_Paren(std::move(paren)), m_Arguments(std::move(arguments)) {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitCallExpr(*this);
    }
};

class AnonFunction : public Expr {
public:
    std::vector<Token> m_Params;
    std::vector<std::shared_ptr<Stmt>> m_Body;

    AnonFunction(std::vector<Token>& params, std::vector<std::shared_ptr<Stmt>>& body)
            : m_Params(std::move(params)), m_Body(std::move(body)) {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitAnonFunctionExpr(*this);
    }
};

class Get : public Expr {
public:
    Token m_Name;
    std::shared_ptr<Expr> m_Object;

    Get(Token& name, std::shared_ptr<Expr>& object)
            : m_Name(std::move(name)), m_Object(std::move(object)) {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitGetExpr(*this);
    }
};

class Grouping : public Expr {
public:
    std::shared_ptr<Expr> m_Expression;

    explicit Grouping(std::shared_ptr<Expr>& expression)
                : m_Expression(std::move(expression)) {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitGroupingExpr(*this);
    }
};

class Literal : public Expr {
public:
    std::any m_Literal;

    explicit Literal(std::any literal) : m_Literal{std::move(literal)} {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitLiteralExpr(*this);
    }
};

class Logical : public Expr {
public:
    std::shared_ptr<Expr> m_Left;
    Token m_Operator;
    std::shared_ptr<Expr> m_Right;

    Logical(std::shared_ptr<Expr>& left, Token& operator_, std::shared_ptr<Expr>& right)
                : m_Left(std::move(left)), m_Operator(std::move(operator_)), m_Right(std::move(right)) {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitLogicalExpr(*this);
    }
};

class Set : public Expr {
public:
    std::shared_ptr<Expr> m_Object;
    Token m_Name;
    std::shared_ptr<Expr> m_Value;

    Set(std::shared_ptr<Expr>& object, Token& name, std::shared_ptr<Expr>& value)
            : m_Object(std::move(object)), m_Name(std::move(name)), m_Value(std::move(value)) {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitSetExpr(*this);
    }
};

class Super : public Expr {
public:
    Token m_Keyword;
    Token m_Method;

    Super(Token& keyword, Token& method)
            : m_Keyword(std::move(keyword)), m_Method(std::move(method)) {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitSuperExpr(*this);
    }
};

class This : public Expr {
public:
    Token m_Keyword;

    explicit This(const Token& keyword) : m_Keyword(std::move(keyword)) {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitThisExpr(*this);
    }
};

class Unary : public Expr {
public:
    Token m_Operator;
    std::shared_ptr<Expr> m_Right;

    Unary(Token& operator_, std::shared_ptr<Expr>& right)
            : m_Operator(operator_), m_Right(right) {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitUnaryExpr(*this);
    }
};

class Ternary : public Expr {
public:
    std::shared_ptr<Expr> m_Expr;
    std::shared_ptr<Expr> m_TrueExpr;
    std::shared_ptr<Expr> m_FalseExpr;

    Ternary(std::shared_ptr<Expr>& expr, std::shared_ptr<Expr>& trueExpr, std::shared_ptr<Expr>& falseExpr)
                : m_Expr(std::move(expr)), m_TrueExpr(std::move(trueExpr)), m_FalseExpr(std::move(falseExpr)) {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitTernaryExpr(*this);
    }
};

class Variable : public Expr {
public:
    Token m_VariableName;

    explicit Variable(Token& name)
                : m_VariableName(std::move(name)) {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitVariableExpr(*this);
    }
};