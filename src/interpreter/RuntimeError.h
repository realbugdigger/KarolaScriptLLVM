#pragma once

#include "../lexer/Token.h"
#include <stdexcept>
#include <any>

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

class ReturnException : public RuntimeError
{
public:
    explicit ReturnException(std::any value) : RuntimeError(), value{std::move(value)} {};

    const std::any& getReturnValue() const { return value; }

private:
    std::any value;
};