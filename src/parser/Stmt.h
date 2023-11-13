#pragma once

#include <utility>
#include <vector>
#include <memory>
#include <any>

#include "../lexer/Token.h"
//#include "Expr.h"

template <typename R> class Expr;

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

class Stmt {
public:
    template<typename R>
    class Visitor {
        public:
            virtual R visitBlockStmt(Block<R> expr) = 0;
            virtual R visitClassStmt(Class<R> expr) = 0;
            virtual R visitExpressionStmt(Expression<R> expr) = 0;
            virtual R visitFunctionStmt(Function<R> expr) = 0;
            virtual R visitIfStmt(If<R> expr) = 0;
            virtual R visitPrintStmt(Print<R> expr) = 0;
            virtual R visitReturnStmt(Return<R> expr) = 0;
            virtual R visitVarStmt(Let<R> expr) = 0;
            virtual R visitWhileStmt(While<R> expr) = 0;
            virtual R visitBreakStmt(Break<R> expr) = 0;
    };

    virtual std::any accept(Stmt::Visitor<std::any>& visitor) = 0;
};

template<typename R>
class Block : public Stmt {
public:
    std::vector<Stmt> m_Statements;

    Block(std::vector<Stmt>& statements)
            : m_Statements(std::move(statements)) {
    }

    std::any accept(typename Stmt::Visitor<std::any>& visitor) override {
        return visitor.visitBlockStmt(*this);
    }
};

template<typename R>
class Class : public Stmt {
public:
    Token m_Name;
    std::shared_ptr<Let<R>> m_Superclass;
    std::vector<Function<R>> m_Methods;
    std::vector<Function<R>> m_ClassMethods;

    Class(const Token& name, const std::shared_ptr<Let<R>>& superclass,
          const std::vector<Function<R>>& methods, const std::vector<Function<R>>& classMethods)
            : m_Name(std::move(name)), m_Superclass(std::move(superclass)), m_Methods(std::move(methods)), m_ClassMethods(std::move(classMethods)) {
    }

    std::any accept(typename Stmt::Visitor<std::any>& visitor) override {
        return visitor.visitClassStmt(*this);
    }
};

template<typename R>
class Expression : Stmt {
public:
    std::shared_ptr<Expr<R>> m_Expression;

    Expression(const Expr<R>& expression)
                : m_Expression(std::move(expression)) {
    }

    std::any accept(typename Stmt::Visitor<std::any>& visitor) override {
        return visitor.visitExpressionStmt(*this);
    }
};

template<typename R>
class Function : Stmt {
public:
    Token m_Name;
    std::vector<Token> m_Params;
    std::vector<Stmt> m_Body;

    Function(const Token& name, std::vector<Token>& params, std::vector<Stmt>& body)
                : m_Name(std::move(name)), m_Params(std::move(params)), m_Body(std::move(body)) {
    }

    std::any accept(typename Stmt::Visitor<std::any>& visitor) override {
        return visitor.visitFunctionStmt(*this);
    }
};

template<typename R>
class If : Stmt {
public:
    std::shared_ptr<Expr<R>> m_Condition;
    std::shared_ptr<Stmt> m_ThenBranch;
    std::shared_ptr<Stmt> m_ElseBranch;

    If(const std::shared_ptr<Expr<R>>& condition, std::shared_ptr<Stmt>& thenBranch, std::shared_ptr<Stmt>& elseBranch)
        : m_Condition(std::move(condition)), m_ThenBranch(std::move(thenBranch)), m_ElseBranch(std::move(elseBranch)) {
    }

    std::any accept(typename Stmt::Visitor<std::any>& visitor) override {
        return visitor.visitIfStmt(*this);
    }
};

template<typename R>
class Print : Stmt {
public:
    std::shared_ptr<Expr<R>> m_Expression;

    Print(const Expr<R>& expression)
            : m_Expression(std::move(expression)) {
    }

    std::any accept(typename Stmt::Visitor<std::any>& visitor) override {
        return visitor.visitPrintStmt(*this);
    }
};

template<typename R>
class Return : Stmt {
public:
    Token m_Keyword;
    std::shared_ptr<Expr<R>> m_Value;

    Return(const Token& keyword, const Expr<R>& value)
            : m_Keyword(std::move(keyword)), m_Value(std::move(value)) {
    }

    std::any accept(typename Stmt::Visitor<std::any>& visitor) override {
        return visitor.visitReturnStmt(*this);
    }
};

template<typename R>
class Let : Stmt {
public:
    Token m_Name;
    std::shared_ptr<Expr<R>> m_Initializer;

    Let(const Token& name, const Expr<R>& initializer)
        : m_Name(std::move(name)), m_Initializer(std::move(initializer)) {
    }

    std::any accept(typename Stmt::Visitor<std::any>& visitor) override {
        return visitor.visitVarStmt(*this);
    }
};

template<typename R>
class While : Stmt {
public:
    std::shared_ptr<Expr<R>> m_Condition;
    std::shared_ptr<Stmt> m_Body;

    While(const std::shared_ptr<Expr<R>>& condition, std::shared_ptr<Stmt>& body)
        : m_Condition(std::move(condition)), m_Body(std::move(body)) {
    }

    std::any accept(typename Stmt::Visitor<std::any>& visitor) override {
        return visitor.visitWhileStmt(*this);
    }
};

template<typename R>
class Break : public Stmt {
public:
    Token m_Keyword;

    Break(const Token& keyword)
        : m_Keyword(std::move(keyword)) {
    }

    std::any accept(typename Stmt::Visitor<std::any>& visitor) override {
        return visitor.visitBreakStmt(*this);
    }
};