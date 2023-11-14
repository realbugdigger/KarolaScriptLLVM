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
    virtual R visitAssignExpr(Assign expr) = 0;
    virtual R visitBinaryExpr(Binary expr) = 0;
    virtual R visitCallExpr(Call expr) = 0;
    virtual R visitAnonFunctionExpr(AnonFunction expr) = 0;
    virtual R visitGetExpr(Get expr) = 0;
    virtual R visitGroupingExpr(Grouping expr) = 0;
    virtual R visitLogicalExpr(Logical expr) = 0;
    virtual R visitSetExpr(Set expr) = 0;
    virtual R visitSuperExpr(Super expr) = 0;
    virtual R visitThisExpr(This expr) = 0;
    virtual R visitUnaryExpr(Unary expr) = 0;
    virtual R visitTernaryExpr(Ternary expr) = 0;
    virtual R visitVariableExpr(Variable expr) = 0;
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

class Binary : Expr {
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

class Call : Expr {
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

class AnonFunction : Expr {
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

class Get : Expr {
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

class Grouping : Expr {
public:
    std::shared_ptr<Expr> m_Expression;

    Grouping(std::shared_ptr<Expr>& expression)
                : m_Expression(std::move(expression)) {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitGroupingExpr(*this);
    }
};

class Logical : Expr {
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

class Set : Expr {
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

class Super : Expr {
public:
    std::vector<Token> m_Keyword;
    std::vector<Token> m_Method;

    Super(std::vector<Token>& keywords, std::vector<Token>& methods)
            : m_Keyword(std::move(keywords)), m_Method(std::move(methods)) {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitSuperExpr(*this);
    }
};

class This : Expr {
public:
    Token m_Keyword;

    This(Token& keyword) : m_Keyword(std::move(keyword)) {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitThisExpr(*this);
    }
};

class Unary : Expr {
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

class Ternary : Expr {
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

class Variable : Expr {
public:
    Token m_VariableName;

    Variable(Token& name)
                : m_VariableName(name) {
    }

    std::any accept(ExprVisitor<std::any>& visitor) override {
        return visitor.visitVariableExpr(*this);
    }
};