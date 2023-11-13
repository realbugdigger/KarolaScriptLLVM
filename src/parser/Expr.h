#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "../lexer/Token.h"
#include "Stmt.h"

template<typename R>
class Assign;
template<typename R>
class Binary;
template<typename R>
class Call;
template<typename R>
class AnonFunction;
template<typename R>
class Get;
template<typename R>
class Grouping;
template<typename R>
class Logical;
template<typename R>
class Set;
template<typename R>
class Super;
template<typename R>
class This;
template<typename R>
class Unary;
template<typename R>
class Ternary;
template<typename R>
class Variable;

template<typename R>
class Expr {
public:
    //template<typename R>
    class Visitor {
        R visitAssignExpr(Assign<R> expr) = 0;
        R visitBinaryExpr(Binary<R> expr) = 0;
        R visitCallExpr(Call<R> expr) = 0;
        R visitAnonFunctionExpr(AnonFunction<R> expr) = 0;
        R visitGetExpr(Get<R> expr) = 0;
        R visitGroupingExpr(Grouping<R> expr) = 0;
        R visitLogicalExpr(Logical<R> expr) = 0;
        R visitSetExpr(Set<R> expr) = 0;
        R visitSuperExpr(Super<R> expr) = 0;
        R visitThisExpr(This<R> expr) = 0;
        R visitUnaryExpr(Unary<R> expr) = 0;
        R visitTernaryExpr(Ternary<R> expr) = 0;
        R visitVariableExpr(Variable<R> expr) = 0;
    };

    //template<typename R>
    virtual R accept(typename Expr<R>::Visitor& visitor) = 0;
};

template<typename R>
class Assign : public Expr<R> {
public:
    Token m_Name;
    std::shared_ptr<Expr<R>> m_Value;

    Assign(const Token& name, const Expr<R>& value)
            : m_Name(std::move(name)), m_Value(std::move(value)) {
    }

    R accept(typename Expr<R>::Visitor& visitor) override {
        return visitor.visitAssignExpr(*this);
    }
};

template<typename R>
class Binary : Expr<R> {
public:
    std::shared_ptr<Expr<R>> m_Left;
    Token m_Operator;
    std::shared_ptr<Expr<R>> m_Right;

    Binary(const Expr<R>& left, const Token& operator_, const Expr<R>& right)
            : m_Left(std::move(left)), m_Operator(std::move(operator_)), m_Right(std::move(right)) {
    }

    R accept(typename Expr<R>::Visitor& visitor) override {
        return visitor.visitBinaryExpr(*this);
    }
};

template<typename R>
class Call : Expr<R> {
public:
    std::shared_ptr<Expr<R>> m_Callee;
    Token m_Paren;
    std::vector<Expr<R>> m_Arguments;

    Call(const Expr<R>& callee, const Token& paren, const std::vector<Expr<R>>& arguments)
            : m_Callee(std::move(callee)), m_Paren(std::move(paren)), m_Arguments(std::move(arguments)) {
    }

    R accept(typename Expr<R>::Visitor& visitor) override {
        return visitor.visitCallExpr(*this);
    }
};

template<typename R>
class AnonFunction : Expr<R> {
public:
    std::vector<Token> m_Params;
    std::vector<Stmt<R>> m_Body;

    AnonFunction(std::vector<Token>& params, std::vector<Stmt<R>>& body)
            : m_Params(std::move(params)), m_Body(std::move(body)) {
    }

    R accept(typename Expr<R>::Visitor& visitor) override {
        return visitor.visitAnonFunctionExpr(*this);
    }
};

template<typename R>
class Get : Expr<R> {
public:
    Token m_Name;
    std::shared_ptr<Expr<R>> m_Object;

    Get(const Token& name, const Expr<R>& object)
            : m_Name(std::move(name)), m_Object(std::move(object)) {
    }

    R accept(typename Expr<R>::Visitor& visitor) override {
        return visitor.visitGetExpr(*this);
    }
};

template<typename R>
class Grouping : Expr<R> {
public:
    std::shared_ptr<Expr<R>> m_Expression;

    Grouping(const Expr<R>& expression)
                : m_Expression(std::move(expression)) {
    }

    R accept(typename Expr<R>::Visitor& visitor) override {
        return visitor.visitGroupingExpr(*this);
    }
};

template<typename R>
class Logical : Expr<R> {
public:
    std::shared_ptr<Expr<R>> m_Left;
    Token m_Operator;
    std::shared_ptr<Expr<R>> m_Right;

    Logical(const Expr<R>& left, const Token& operator_, const Expr<R>& right)
                : m_Left(std::move(left)), m_Operator(std::move(operator_)), m_Right(std::move(right)) {
    }

    R accept(typename Expr<R>::Visitor& visitor) override {
        return visitor.visitLogicalExpr(*this);
    }
};

template<typename R>
class Set : Expr<R> {
public:
    std::shared_ptr<Expr<R>> m_Object;
    Token m_Name;
    std::shared_ptr<Expr<R>> m_Value;

    Set(const Expr<R>& object, const Token& name, const Expr<R>& value)
            : m_Object(std::move(object)), m_Name(std::move(name)), m_Value(std::move(value)) {
    }

    R accept(typename Expr<R>::Visitor& visitor) override {
        return visitor.visitSetExpr(*this);
    }
};

template<typename R>
class Super : Expr<R> {
public:
    std::vector<Token> m_Keyword;
    std::vector<Token> m_Method;

    Super(std::vector<Token>& keywords, std::vector<Token>& methods)
            : m_Keyword(std::move(keywords)), m_Method(std::move(methods)) {
    }

    R accept(typename Expr<R>::Visitor& visitor) override {
        return visitor.visitSuperExpr(*this);
    }
};

template<typename R>
class This : Expr<R> {
public:
    Token m_Keyword;

    This(const Token& keyword) : m_Keyword(std::move(keyword)) {
    }

    R accept(typename Expr<R>::Visitor& visitor) override {
        return visitor.visitThisExpr(*this);
    }
};

template<typename R>
class Unary : Expr<R> {
public:
    Token m_Operator;
    std::shared_ptr<Expr<R>> m_Right;

    Unary(const Token& operator_, const Expr<R>& right)
            : m_Operator(operator_), m_Right(right) {
    }

    R accept(typename Expr<R>::Visitor& visitor) override {
        return visitor.visitUnaryExpr(*this);
    }
};

template<typename R>
class Ternary : Expr<R> {
public:
    std::shared_ptr<Expr<R>> m_Expr;
    std::shared_ptr<Expr<R>> m_TrueExpr;
    std::shared_ptr<Expr<R>> m_FalseExpr;

    Ternary(const Expr<R>& expr, const Expr<R>& trueExpr, const Expr<R>& falseExpr)
                : m_Expr(std::move(expr)), m_TrueExpr(std::move(trueExpr)), m_FalseExpr(std::move(falseExpr)) {
    }

    R accept(typename Expr<R>::Visitor& visitor) override {
        return visitor.visitTernaryExpr(*this);
    }
};

template<typename R>
class Variable : Expr<R> {
public:
    Token m_VariableName;

    Variable(const Token& name)
                : m_VariableName(name) {
    }

    R accept(typename Expr<R>::Visitor& visitor) override {
        return visitor.visitVariableExpr(*this);
    }
};