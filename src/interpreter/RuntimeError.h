#pragma once

#include "../lexer/Token.h"
#include <stdexcept>
#include <any>
#include <memory>

class RuntimeError : std::runtime_error {
private:
    Token token;
public:
    explicit RuntimeError() : std::runtime_error{""} {}

    RuntimeError(const Token& token, const std::string& message)
            : std::runtime_error{message}, token{token}
    {
    }

    const Token& getToken() const { return token; }
};

class BreakException : public RuntimeError
{
public:
    explicit BreakException(const Token& token) noexcept
            : RuntimeError(token, "Cannot break outside of a loop."){};
};

class ReturnException : public RuntimeError {
public:
    std::shared_ptr<std::any> m_Value;
public:
    explicit ReturnException(std::shared_ptr<std::any> value) : RuntimeError(), m_Value{std::move(value)} {};

    //const std::any& getReturnValue() const { return m_Value; }
};