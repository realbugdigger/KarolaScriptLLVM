#pragma once

#include <utility>

#include "Expr.h"

template<typename R>
class Block;
template<typename R>
class Class;
template<typename R>
class Expression;
template<typename R>
class Function;
template<typename R>
class If;
template<typename R>
class Print;
template<typename R>
class Return;
template<typename R>
class Let;
template<typename R>
class While;
template<typename R>
class Break;

template<typename R>
class Stmt {
public:
    //template<typename R>
    class Visitor {
        R visitBlockStmt(Block<R> expr) = 0;
        R visitClassStmt(Class<R> expr) = 0;
        R visitExpressionStmt(Expression<R> expr) = 0;
        R visitFunctionStmt(Function<R> expr) = 0;
        R visitIfStmt(If<R> expr) = 0;
        R visitPrintStmt(Print<R> expr) = 0;
        R visitReturnStmt(Return<R> expr) = 0;
        R visitVarStmt(Let<R> expr) = 0;
        R visitWhileStmt(While<R> expr) = 0;
        R visitBreakStmt(Break<R> expr) = 0;
    };

    //template<typename R>
    virtual R accept(typename Stmt<R>::Visitor& visitor) = 0;
};

template<typename R>
class Block : public Stmt<R> {
public:
    std::vector<Stmt<R>> m_Statements;

    Block(const std::vector<Stmt<R>>& statements)
            : m_Statements(std::move(statements)) {
    }

    R accept(typename Stmt<R>::Visitor& visitor) override {
        return visitor.visitBlockStmt(*this);
    }
};

template<typename R>
class Class : public Stmt<R> {
public:
    Token m_Name;
    std::shared_ptr<Let<R>> m_Superclass;
    std::vector<Function<R>> m_Methods;
    std::vector<Function<R>> m_ClassMethods;

    Class(const Token& name, const std::shared_ptr<Let<R>>& superclass,
          const std::vector<Function<R>>& methods, const std::vector<Function<R>>& classMethods)
            : m_Name(std::move(name)), m_Superclass(std::move(superclass)), m_Methods(std::move(methods)), m_ClassMethods(std::move(classMethods)) {
    }

    R accept(typename Stmt<R>::Visitor& visitor) override {
        return visitor.visitClassStmt(*this);
    }
};

template<typename R>
class Expression : Stmt<R> {
public:
    std::shared_ptr<Expr<R>> m_Expression;

    Expression(const Expr<R>& expression)
                : m_Expression(std::move(expression)) {
    }

    R accept(typename Stmt<R>::Visitor& visitor) override {
        return visitor.visitExpressionStmt(*this);
    }
};

template<typename R>
class Function : Stmt<R> {
public:
    Token m_Name;
    std::vector<Token> m_Params;
    std::vector<Stmt<R>> m_Body;

    Function(const Token& name, std::vector<Token>& params, const std::vector<Stmt<R>>& body)
                : m_Name(std::move(name)), m_Params(std::move(params)), m_Body(std::move(body)) {
    }

    R accept(typename Stmt<R>::Visitor& visitor) override {
        return visitor.visitFunctionStmt(*this);
    }
};

template<typename R>
class If : Stmt<R> {
public:
    std::shared_ptr<Expr<R>> m_Condition;
    std::shared_ptr<Stmt<R>> m_ThenBranch;
    std::shared_ptr<Stmt<R>> m_ElseBranch;

    If(const std::shared_ptr<Expr<R>>& condition, const std::shared_ptr<Stmt<R>>& thenBranch, const std::shared_ptr<Stmt<R>>& elseBranch)
        : m_Condition(std::move(condition)), m_ThenBranch(std::move(thenBranch)), m_ElseBranch(std::move(elseBranch)) {
    }

    R accept(typename Stmt<R>::Visitor& visitor) override {
        return visitor.visitIfStmt(*this);
    }
};

template<typename R>
class Print : Stmt<R> {
public:
    std::shared_ptr<Expr<R>> m_Expression;

    Print(const Expr<R>& expression)
            : m_Expression(std::move(expression)) {
    }

    R accept(typename Stmt<R>::Visitor& visitor) override {
        return visitor.visitPrintStmt(*this);
    }
};

template<typename R>
class Return : Stmt<R> {
public:
    Token m_Keyword;
    std::shared_ptr<Expr<R>> m_Value;

    Return(const Token& keyword, const Expr<R>& value)
            : m_Keyword(std::move(keyword)), m_Value(std::move(value)) {
    }

    R accept(typename Stmt<R>::Visitor& visitor) override {
        return visitor.visitReturnStmt(*this);
    }
};

template<typename R>
class Let : Stmt<R> {
public:
    Token m_Name;
    std::shared_ptr<Expr<R>> m_Initializer;

    Let(const Token& name, const Expr<R>& initializer)
        : m_Name(std::move(name)), m_Initializer(std::move(initializer)) {
    }

    R accept(typename Stmt<R>::Visitor& visitor) override {
        return visitor.visitVarStmt(*this);
    }
};

template<typename R>
class While : Stmt<R> {
public:
    std::shared_ptr<Expr<R>> m_Condition;
    std::shared_ptr<Stmt<R>> m_Body;

    While(const std::shared_ptr<Expr<R>>& condition, const std::shared_ptr<Stmt<R>>& body)
        : m_Condition(std::move(condition)), m_Body(std::move(body)) {
    }

    R accept(typename Stmt<R>::Visitor& visitor) override {
        return visitor.visitWhileStmt(*this);
    }
};

template<typename R>
class Break : public Stmt<R> {
public:
    Token m_Keyword;

    Break(const Token& keyword)
        : m_Keyword(std::move(keyword)) {
    }

    R accept(typename Stmt<R>::Visitor& visitor) override {
        return visitor.visitBreakStmt(*this);
    }
};