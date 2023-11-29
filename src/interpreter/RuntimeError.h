#pragma once

#include <stdexcept>
#include <any>
#include <memory>

#include "../lexer/Token.h"
#include "../util/Object.h"

class RuntimeError : std::runtime_error {
private:
    std::string message;
    Token token;
public:
    explicit RuntimeError() : std::runtime_error{""} {}

    RuntimeError(const Token& token, const std::string& message, int line = -1)
            : std::runtime_error{message}, token{token}
    {
        if (line != -1){
            this->message = "[Line " + std::to_string(line) + "] " + message;
        } else {
            this->message = "" + message;
        }
    }

    explicit RuntimeError(const std::string& message, int line = -1)
            : std::runtime_error{message}, token{token}
    {
        if (line != -1){
            this->message = "[Line " + std::to_string(line) + "] " + message;
        } else {
            this->message = message;
        }
    }

    const Token& getToken() const { return token; }

    const std::string getMessage() const { return message; }
};

class BreakException : public RuntimeError
{
public:
    explicit BreakException(const Token& token) noexcept
            : RuntimeError(token, "Cannot break outside of a loop."){};
};

class ReturnException : public RuntimeError {
public:
    Object m_Value;
public:
    explicit ReturnException(Object value) : RuntimeError(), m_Value{std::move(value)} {};

    //const Object& getReturnValue() const { return m_Value; }
};