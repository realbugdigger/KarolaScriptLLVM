#pragma once

#include <memory>
#include <utility>
#include <vector>
#include <any>

#include "../lexer/Token.h"
//#include "Stmt.h"

class Stmt;

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

class Expr {
public:
    template<typename R>
    class Visitor {
        public:
            virtual R visitAssignExpr(Assign<R> expr) = 0;
            virtual R visitBinaryExpr(Binary<R> expr) = 0;
            virtual R visitCallExpr(Call<R> expr) = 0;
            virtual R visitAnonFunctionExpr(AnonFunction<R> expr) = 0;
            virtual R visitGetExpr(Get<R> expr) = 0;
            virtual R visitGroupingExpr(Grouping<R> expr) = 0;
            virtual R visitLogicalExpr(Logical<R> expr) = 0;
            virtual R visitSetExpr(Set<R> expr) = 0;
            virtual R visitSuperExpr(Super<R> expr) = 0;
            virtual R visitThisExpr(This<R> expr) = 0;
            virtual R visitUnaryExpr(Unary<R> expr) = 0;
            virtual R visitTernaryExpr(Ternary<R> expr) = 0;
            virtual R visitVariableExpr(Variable<R> expr) = 0;
    };

    virtual std::any accept(typename Expr::Visitor<std::any>& visitor) = 0;
};

template<typename R>
class Assign : public Expr {
public:
    Token m_Name;
    std::shared_ptr<Expr> m_Value;

    Assign(Token& name, std::shared_ptr<Expr>& value)
            : m_Name(std::move(name)), m_Value(std::move(value)) {
    }

    std::any accept(typename Expr::Visitor<std::any>& visitor) override {
        return visitor.visitAssignExpr(*this);
    }
};

template<typename R>
class Binary : Expr {
public:
    std::shared_ptr<Expr> m_Left;
    Token m_Operator;
    std::shared_ptr<Expr> m_Right;

    Binary(std::shared_ptr<Expr>& left, Token& operator_, std::shared_ptr<Expr>& right)
            : m_Left(std::move(left)), m_Operator(std::move(operator_)), m_Right(std::move(right)) {
    }

    std::any accept(typename Expr::Visitor<std::any>& visitor) override {
        return visitor.visitBinaryExpr(*this);
    }
};

template<typename R>
class Call : Expr {
public:
    std::shared_ptr<Expr> m_Callee;
    Token m_Paren;
    std::vector<Expr> m_Arguments;

    Call(std::shared_ptr<Expr>& callee, Token& paren, std::vector<Expr>& arguments)
            : m_Callee(std::move(callee)), m_Paren(std::move(paren)), m_Arguments(std::move(arguments)) {
    }

    std::any accept(typename Expr::Visitor<std::any>& visitor) override {
        return visitor.visitCallExpr(*this);
    }
};

template<typename R>
class AnonFunction : Expr {
public:
    std::vector<Token> m_Params;
    std::vector<Stmt> m_Body;

    AnonFunction(std::vector<Token>& params, std::vector<Stmt>& body)
            : m_Params(std::move(params)), m_Body(std::move(body)) {
    }

    std::any accept(typename Expr::Visitor<std::any>& visitor) override {
        return visitor.visitAnonFunctionExpr(*this);
    }
};

template<typename R>
class Get : Expr {
public:
    Token m_Name;
    std::shared_ptr<Expr> m_Object;

    Get(Token& name, std::shared_ptr<Expr>& object)
            : m_Name(std::move(name)), m_Object(std::move(object)) {
    }

    std::any accept(typename Expr::Visitor<std::any>& visitor) override {
        return visitor.visitGetExpr(*this);
    }
};

template<typename R>
class Grouping : Expr {
public:
    std::shared_ptr<Expr> m_Expression;

    Grouping(std::shared_ptr<Expr>& expression)
                : m_Expression(std::move(expression)) {
    }

    std::any accept(typename Expr::Visitor<std::any>& visitor) override {
        return visitor.visitGroupingExpr(*this);
    }
};

template<typename R>
class Logical : Expr {
public:
    std::shared_ptr<Expr> m_Left;
    Token m_Operator;
    std::shared_ptr<Expr> m_Right;

    Logical(std::shared_ptr<Expr>& left, Token& operator_, std::shared_ptr<Expr>& right)
                : m_Left(std::move(left)), m_Operator(std::move(operator_)), m_Right(std::move(right)) {
    }

    std::any accept(typename Expr::Visitor<std::any>& visitor) override {
        return visitor.visitLogicalExpr(*this);
    }
};

template<typename R>
class Set : Expr {
public:
    std::shared_ptr<Expr> m_Object;
    Token m_Name;
    std::shared_ptr<Expr> m_Value;

    Set(std::shared_ptr<Expr>& object, Token& name, std::shared_ptr<Expr>& value)
            : m_Object(std::move(object)), m_Name(std::move(name)), m_Value(std::move(value)) {
    }

    std::any accept(typename Expr::Visitor<std::any>& visitor) override {
        return visitor.visitSetExpr(*this);
    }
};

template<typename R>
class Super : Expr {
public:
    std::vector<Token> m_Keyword;
    std::vector<Token> m_Method;

    Super(std::vector<Token>& keywords, std::vector<Token>& methods)
            : m_Keyword(std::move(keywords)), m_Method(std::move(methods)) {
    }

    std::any accept(typename Expr::Visitor<std::any>& visitor) override {
        return visitor.visitSuperExpr(*this);
    }
};

template<typename R>
class This : Expr {
public:
    Token m_Keyword;

    This(Token& keyword) : m_Keyword(std::move(keyword)) {
    }

    std::any accept(typename Expr::Visitor<std::any>& visitor) override {
        return visitor.visitThisExpr(*this);
    }
};

template<typename R>
class Unary : Expr {
public:
    Token m_Operator;
    std::shared_ptr<Expr> m_Right;

    Unary(Token& operator_, std::shared_ptr<Expr>& right)
            : m_Operator(operator_), m_Right(right) {
    }

    std::any accept(typename Expr::Visitor<std::any>& visitor) override {
        return visitor.visitUnaryExpr(*this);
    }
};

template<typename R>
class Ternary : Expr {
public:
    std::shared_ptr<Expr> m_Expr;
    std::shared_ptr<Expr> m_TrueExpr;
    std::shared_ptr<Expr> m_FalseExpr;

    Ternary(std::shared_ptr<Expr>& expr, std::shared_ptr<Expr>& trueExpr, std::shared_ptr<Expr>& falseExpr)
                : m_Expr(std::move(expr)), m_TrueExpr(std::move(trueExpr)), m_FalseExpr(std::move(falseExpr)) {
    }

    std::any accept(typename Expr::Visitor<std::any>& visitor) override {
        return visitor.visitTernaryExpr(*this);
    }
};

template<typename R>
class Variable : Expr {
public:
    Token m_VariableName;

    Variable(Token& name)
                : m_VariableName(name) {
    }

    std::any accept(typename Expr::Visitor<std::any>& visitor) override {
        return visitor.visitVariableExpr(*this);
    }
};