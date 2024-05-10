#pragma once

#include <memory>
#include <vector>
#include <utility>

#include "../lexer/Token.h"
#include "../util/Object.h"
#include "../util/common.h"

//class Stmt;

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
    virtual R visitAssignExpr(Assign& expr) = 0;
    virtual R visitBinaryExpr(Binary& expr) = 0;
    virtual R visitCallExpr(Call& expr) = 0;
    virtual R visitAnonFunctionExpr(AnonFunction& expr) = 0;
    virtual R visitGetExpr(Get& expr) = 0;
    virtual R visitGroupingExpr(Grouping& expr) = 0;
    virtual R visitLiteralExpr(Literal& expr) = 0;
    virtual R visitLogicalExpr(Logical& expr) = 0;
    virtual R visitSetExpr(Set& expr) = 0;
    virtual R visitSuperExpr(Super& expr) = 0;
    virtual R visitThisExpr(This& expr) = 0;
    virtual R visitUnaryExpr(Unary& expr) = 0;
    virtual R visitTernaryExpr(Ternary& expr) = 0;
    virtual R visitVariableExpr(Variable& expr) = 0;
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual Object accept(ExprVisitor<Object>& visitor) = 0;
    virtual llvm::Value *codegen(llvm::IRBuilder<>& builder) = 0;

    /// ?????

    /// Analyzes the semantics of current node and return a new node in case
    /// that we need to semantically rewrite the current node and replace it with
    /// another node. For example to change from a List containing `(def a b)`
    /// to a `Def` node that represents defining a new binding.
    ///
    /// \param ctx is the context object of the semantic analyzer.
    virtual UniqueStmtPtr analyze(KarolaScriptContext &ctx) = 0; // this is actually inside Resolver.h so we can comment it out probably
    // to confirm look again at tutorial 06 (Yt series - How to write compiler with LLVM and MLIR)

    /// Genenates the correspondig KSIR of the expressoin and attach it to the
    /// module of the given namespace.
    ///
    /// \param ns The namespace that current expression is in it.
    virtual void generateIR(KarolaScriptNamespace &ns) = 0;
};

class Assign : public Expr {
public:
    Token m_Name;
    UniqueExprPtr m_Value;

    Assign(const Token& name, UniqueExprPtr value)
            : m_Name(name), m_Value(std::move(value)) {
    }

    Object accept(ExprVisitor<Object>& visitor) override {
        return visitor.visitAssignExpr(*this);
    }
};

class Binary : public Expr {
public:
    UniqueExprPtr m_Left;
    Token m_Operator;
    UniqueExprPtr m_Right;

    Binary(UniqueExprPtr left, const Token& operator_, UniqueExprPtr right)
            : m_Left(std::move(left)), m_Operator(operator_), m_Right(std::move(right)) {
    }

    Object accept(ExprVisitor<Object>& visitor) override {
        return visitor.visitBinaryExpr(*this);
    }
};

class Call : public Expr {
public:
    UniqueExprPtr m_Callee;
    Token m_Paren;
    std::vector<UniqueExprPtr> m_Arguments;

    Call(UniqueExprPtr callee, const Token& paren, std::vector<UniqueExprPtr> arguments)
            : m_Callee(std::move(callee)), m_Paren(paren), m_Arguments(std::move(arguments)) {
    }

    Object accept(ExprVisitor<Object>& visitor) override {
        return visitor.visitCallExpr(*this);
    }
};

class AnonFunction : public Expr {
public:
    std::vector<Token> m_Params;
    std::vector<UniqueStmtPtr> m_Body;

    AnonFunction(std::vector<Token> params, std::vector<UniqueStmtPtr> body)
            : m_Params(std::move(params)), m_Body(std::move(body)) {}

    Object accept(ExprVisitor<Object>& visitor) override {
        return visitor.visitAnonFunctionExpr(*this);
    }
};

class Get : public Expr {
public:
    /*VariableExpr that refers to the object (not the field!) that is being accessed. For example if the parsed code were
     * 'obj.a' then this variable would hold a pointer to 'obj' */
    UniqueExprPtr m_Object;
    /*Token of the identifier of the field being accessed. If the parsed code were 'obj.a' then this variable would contain
     * the token corresponding to 'a' */
    Token m_Name;

    Get(const Token& name, UniqueExprPtr object)
            : m_Name(name), m_Object(std::move(object)) {}

    Object accept(ExprVisitor<Object>& visitor) override {
        return visitor.visitGetExpr(*this);
    }
};

class Grouping : public Expr {
public:
    UniqueExprPtr m_Expression;

    explicit Grouping(UniqueExprPtr expression)
                : m_Expression(std::move(expression)) {}

    Object accept(ExprVisitor<Object>& visitor) override {
        return visitor.visitGroupingExpr(*this);
    }
};

class Literal : public Expr {
public:
    Object m_Literal;

    explicit Literal(const Object& literal) : m_Literal(literal) {}

    Object accept(ExprVisitor<Object>& visitor) override {
        return visitor.visitLiteralExpr(*this);
    }
};

class Logical : public Expr {
public:
    UniqueExprPtr m_Left;
    Token m_Operator;
    UniqueExprPtr m_Right;

    Logical(UniqueExprPtr left, const Token& operator_, UniqueExprPtr right)
                : m_Left(std::move(left)), m_Operator(operator_), m_Right(std::move(right)) {
    }

    Object accept(ExprVisitor<Object>& visitor) override {
        return visitor.visitLogicalExpr(*this);
    }
};

class Set : public Expr {
public:
    /*VariableExpr that refers to the object (not the field!) that is being accessed. For example if the parsed code were
     * 'obj.a' then this variable would hold a pointer to 'obj' */
    UniqueExprPtr m_Object;
    /*Token of the identifier of the field being accessed. If the parsed code were 'obj.a' then this variable would contain
     * the token corresponding to 'a' */
    Token m_Name;
    UniqueExprPtr m_Value;

    Set(UniqueExprPtr object, const Token& name, UniqueExprPtr value)
            : m_Object(std::move(object)), m_Name(name), m_Value(std::move(value)) {
    }

    Object accept(ExprVisitor<Object>& visitor) override {
        return visitor.visitSetExpr(*this);
    }
};

class Super : public Expr {
public:
    Token m_Keyword;
    Token m_Method;

    Super(const Token& keyword, const Token& method)
            : m_Keyword(keyword), m_Method(method) {
    }

    Object accept(ExprVisitor<Object>& visitor) override {
        return visitor.visitSuperExpr(*this);
    }
};

class This : public Expr {
public:
    Token m_Keyword;

    explicit This(const Token& keyword) : m_Keyword(keyword) {
    }

    Object accept(ExprVisitor<Object>& visitor) override {
        return visitor.visitThisExpr(*this);
    }
};

class Unary : public Expr {
public:
    Token m_Operator;
    UniqueExprPtr m_Right;

    Unary(const Token& operator_, UniqueExprPtr right)
            : m_Operator(operator_), m_Right(std::move(right)) {
    }

    Object accept(ExprVisitor<Object>& visitor) override {
        return visitor.visitUnaryExpr(*this);
    }
};

class Ternary : public Expr {
public:
    UniqueExprPtr m_Expr;
    UniqueExprPtr m_TrueExpr;
    UniqueExprPtr m_FalseExpr;

    Ternary(UniqueExprPtr expr, UniqueExprPtr trueExpr, UniqueExprPtr falseExpr)
                : m_Expr(std::move(expr)), m_TrueExpr(std::move(trueExpr)), m_FalseExpr(std::move(falseExpr)) {
    }

    Object accept(ExprVisitor<Object>& visitor) override {
        return visitor.visitTernaryExpr(*this);
    }
};

class Variable : public Expr {
public:
    Token m_VariableName;

    explicit Variable(const Token& name)
                : m_VariableName(name) {
    }

    Object accept(ExprVisitor<Object>& visitor) override {
        return visitor.visitVariableExpr(*this);
    }
};