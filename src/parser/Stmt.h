#pragma once

#include <utility>
#include <vector>
#include <memory>

#include "../lexer/Token.h"
#include "../util/common.h"

//class Expr;

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
    virtual void visitBlockStmt(Block& stmt) = 0;
    virtual void visitClazzStmt(Class& stmt) = 0;
    virtual void visitExpressionStmt(Expression& stmt) = 0;
    virtual void visitFunctionStmt(Function& stmt) = 0;
    virtual void visitIfStmt(If& stmt) = 0;
    virtual void visitPrintStmt(Print& stmt) = 0;
    virtual void visitReturnStmt(Return& stmt) = 0;
    virtual void visitLetStmt(Let& stmt) = 0;
    virtual void visitWhileStmt(While& stmt) = 0;
    virtual void visitBreakStmt(Break& stmt) = 0;
};

class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) = 0;
};

class Block : public Stmt {
public:
    std::vector<UniqueStmtPtr> m_Statements;

    explicit Block(std::vector<UniqueStmtPtr> statements)
            : m_Statements(std::move(statements)) {
    }

    void accept(StmtVisitor& visitor) override {
        return visitor.visitBlockStmt(*this);
    }
};

class Class : public Stmt {
public:
    Token m_Name;
    std::optional<std::unique_ptr<Variable>> m_Superclass; //Superclass is a Variable expression instead of a Token because the resolver needs to resolve the superclass and it needs an expr to do so.
    std::vector<std::unique_ptr<Function>> m_Methods;
    std::vector<std::unique_ptr<Function>> m_ClassMethods;

    Class(const Token& name, std::optional<std::unique_ptr<Variable>> superclass, std::vector<std::unique_ptr<Function>> methods, std::vector<std::unique_ptr<Function>> classMethods)
            : m_Name(name), m_Superclass(std::move(superclass)), m_Methods(std::move(methods)), m_ClassMethods(std::move(classMethods)) {
    }

    void accept(StmtVisitor& visitor) override {
        return visitor.visitClazzStmt(*this);
    }
};

class Expression : public Stmt {
public:
    UniqueExprPtr m_Expression;

    explicit Expression(UniqueExprPtr expression) : m_Expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitExpressionStmt(*this);
    }
};

class Function : public Stmt {
public:
    Token m_Name;
    std::vector<Token> m_Params;
    std::vector<UniqueStmtPtr> m_Body;

    Function(const Token& name, const std::vector<Token>& params, std::vector<UniqueStmtPtr> body)
                : m_Name(name), m_Params(params), m_Body(std::move(body)) {
    }

    void accept(StmtVisitor& visitor) override {
        visitor.visitFunctionStmt(*this);
    }
};

class If : public Stmt {
public:
    UniqueExprPtr m_Condition;
    UniqueStmtPtr m_ThenBranch;
    std::optional<UniqueStmtPtr> m_ElseBranch;

    If(UniqueExprPtr condition, UniqueStmtPtr thenBranch, std::optional<UniqueStmtPtr> elseBranch)
        : m_Condition(std::move(condition)), m_ThenBranch(std::move(thenBranch)), m_ElseBranch(std::move(elseBranch)) {
    }

    void accept(StmtVisitor& visitor) override {
        visitor.visitIfStmt(*this);
    }
};

class Print : public Stmt {
public:
    std::optional<UniqueExprPtr> m_Expression;

    explicit Print(std::optional<UniqueExprPtr> expression)
            : m_Expression(std::move(expression)) {
    }

    void accept(StmtVisitor& visitor) override {
        visitor.visitPrintStmt(*this);
    }
};

class Return : public Stmt {
public:
    Token m_Keyword;
    std::optional<UniqueExprPtr> m_Value;

    Return(const Token& keyword, std::optional<UniqueExprPtr> value)
            : m_Keyword(keyword), m_Value(std::move(value)) {
    }

    void accept(StmtVisitor& visitor) override {
        visitor.visitReturnStmt(*this);
    }
};

class Let : public Stmt {
public:
    Token m_Name;
    std::optional<UniqueExprPtr> m_Initializer; // Optional because you may declare a variable without initializing it.

    Let(const Token& name, std::optional<UniqueExprPtr> initializer)
        : m_Name(name), m_Initializer(std::move(initializer)) {
    }

    void accept(StmtVisitor& visitor) override {
        visitor.visitLetStmt(*this);
    }
};

class While : public Stmt {
public:
    UniqueExprPtr m_Condition;
    UniqueStmtPtr m_Body;

    While(UniqueExprPtr condition, UniqueStmtPtr body)
        : m_Condition(std::move(condition)), m_Body(std::move(body)) {
    }

    void accept(StmtVisitor& visitor) override {
        visitor.visitWhileStmt(*this);
    }
};

class Break : public Stmt {
public:
    Token m_Keyword;

    explicit Break(const Token& keyword)
        : m_Keyword(keyword) {
    }

    void accept(StmtVisitor& visitor) override {
        visitor.visitBreakStmt(*this);
    }
};