#pragma once

#include <utility>
#include <vector>
#include <memory>
#include <any>

#include "../lexer/Token.h"
//#include "Expr.h"

class Expr;

class Block;
class Class;
class Expression;
class Function;
class If;
class Print;
class Return;
class Let;
class While;
class Break;

class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visitBlockStmt(Block expr) = 0;
    virtual void visitClassStmt(Class expr) = 0;
    virtual void visitExpressionStmt(Expression expr) = 0;
    virtual void visitFunctionStmt(Function expr) = 0;
    virtual void visitIfStmt(If expr) = 0;
    virtual void visitPrintStmt(Print expr) = 0;
    virtual void visitReturnStmt(Return expr) = 0;
    virtual void visitVarStmt(Let expr) = 0;
    virtual void visitWhileStmt(While expr) = 0;
    virtual void visitBreakStmt(Break expr) = 0;
};

class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) = 0;
};

class Block : public Stmt {
public:
    std::vector<std::shared_ptr<Stmt>> m_Statements;

    explicit Block(std::vector<std::shared_ptr<Stmt>>& statements)
            : m_Statements(std::move(statements)) {
    }

    void accept(StmtVisitor& visitor) override {
        return visitor.visitBlockStmt(*this);
    }
};

class Class : public Stmt {
public:
    Token m_Name;
    std::shared_ptr<Let> m_Superclass;
    std::vector<Function> m_Methods;
    std::vector<Function> m_ClassMethods;

    Class(Token& name, std::shared_ptr<Let>& superclass,
          std::vector<Function>& methods, std::vector<Function>& classMethods)
            : m_Name(std::move(name)), m_Superclass(std::move(superclass)), m_Methods(std::move(methods)), m_ClassMethods(std::move(classMethods)) {
    }

    void accept(StmtVisitor& visitor) override {
        return visitor.visitClassStmt(*this);
    }
};

class Expression : Stmt {
public:
    std::shared_ptr<Expr> m_Expression;

    explicit Expression(std::shared_ptr<Expr>& expression)
                : m_Expression(std::move(expression)) {
    }

    void accept(StmtVisitor& visitor) override {
        visitor.visitExpressionStmt(*this);
    }
};

class Function : Stmt {
public:
    Token m_Name;
    std::vector<Token> m_Params;
    std::vector<std::shared_ptr<Stmt>> m_Body;

    Function(const Token& name, std::vector<Token>& params, std::vector<std::shared_ptr<Stmt>>& body)
                : m_Name(std::move(name)), m_Params(std::move(params)), m_Body(std::move(body)) {
    }

    void accept(StmtVisitor& visitor) override {
        visitor.visitFunctionStmt(*this);
    }
};

class If : Stmt {
public:
    std::shared_ptr<Expr> m_Condition;
    std::shared_ptr<Stmt> m_ThenBranch;
    std::shared_ptr<Stmt> m_ElseBranch;

    If(const std::shared_ptr<Expr>& condition, std::shared_ptr<Stmt>& thenBranch, std::shared_ptr<Stmt>& elseBranch)
        : m_Condition(std::move(condition)), m_ThenBranch(std::move(thenBranch)), m_ElseBranch(std::move(elseBranch)) {
    }

    void accept(StmtVisitor& visitor) override {
        visitor.visitIfStmt(*this);
    }
};

class Print : Stmt {
public:
    std::shared_ptr<Expr> m_Expression;

    explicit Print(std::shared_ptr<Expr>& expression)
            : m_Expression(std::move(expression)) {
    }

    void accept(StmtVisitor& visitor) override {
        visitor.visitPrintStmt(*this);
    }
};

class Return : Stmt {
public:
    Token m_Keyword;
    std::shared_ptr<Expr> m_Value;

    Return(Token& keyword, std::shared_ptr<Expr>& value)
            : m_Keyword(std::move(keyword)), m_Value(std::move(value)) {
    }

    void accept(StmtVisitor& visitor) override {
        visitor.visitReturnStmt(*this);
    }
};

class Let : Stmt {
public:
    Token m_Name;
    std::shared_ptr<Expr> m_Initializer;

    Let(Token& name, std::shared_ptr<Expr>& initializer)
        : m_Name(std::move(name)), m_Initializer(std::move(initializer)) {
    }

    void accept(StmtVisitor& visitor) override {
        visitor.visitVarStmt(*this);
    }
};

class While : Stmt {
public:
    std::shared_ptr<Expr> m_Condition;
    std::shared_ptr<Stmt> m_Body;

    While(std::shared_ptr<Expr>& condition, std::shared_ptr<Stmt>& body)
        : m_Condition(std::move(condition)), m_Body(std::move(body)) {
    }

    void accept(StmtVisitor& visitor) override {
        visitor.visitWhileStmt(*this);
    }
};

class Break : public Stmt {
public:
    Token m_Keyword;

    explicit Break(Token& keyword)
        : m_Keyword(std::move(keyword)) {
    }

    void accept(StmtVisitor& visitor) override {
        visitor.visitBreakStmt(*this);
    }
};