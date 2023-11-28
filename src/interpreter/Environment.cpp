#include "Environment.h"

#include <cassert>
#include <stdexcept>
#include <utility>

#include "RuntimeError.h"
#include "../util/Object.h"
#include "../lexer/Token.h"

Environment::Environment(std::shared_ptr<Environment> enclosing) : m_Enclosing{std::move(enclosing)} {}

void Environment::define(const Token& identifier, const Object& value) {
    std::string key = identifier.lexeme;
    if (m_Values.count(key) == 1){
        throw RuntimeError("Cannot redefine a variable. Variable '" + key + "' has already been defined", identifier.line);
    }

    m_Values[key] = value;
}

void Environment::define(const std::string &key, const Object& value) {
    if (m_Values.count(key) == 1){
        throw RuntimeError("Cannot redefine a variable. Variable '" + key + "' has already been defined");
    }

    m_Values[key] = value;
}

Object Environment::lookup(const Token& identifier) {
    // Check if the current environment contains the identifier.
    if (m_Values.find(identifier.lexeme) != m_Values.end()) {
        // If so, return the value associated with it.
        return m_Values[identifier.lexeme];
    }

    // If the identifier is not in the current environment, check the parent environment until global scope.
    if (m_Enclosing) {
        return m_Enclosing->lookup(identifier);
    }

    // If not in global scope, throw error.
    throw RuntimeError(identifier, "Undefined variable '" + identifier.lexeme + "'.");
}

Object Environment::lookup(const std::string& identifier) {
    // Check if the current environment contains the identifier.
    if (m_Values.find(identifier) != m_Values.end()) {
        // If so, return the value associated with it.
        return m_Values[identifier];
    }

    // If the identifier is not in the current environment, check the parent environment until global scope.
    if (m_Enclosing) {
        return m_Enclosing->lookup(identifier);
    }

    // If not in global scope, throw error.
    throw RuntimeError("Undefined variable '" + identifier + "'.");
}

Object Environment::getAt(int distance, const std::string& identifier) {
    return ancestor(distance)->m_Values[identifier];
}

void Environment::assign(const Token& identifier, const Object& value) {
    if (m_Values.find(identifier.lexeme) != m_Values.end()) {
        m_Values[identifier.lexeme] = value;
        return;
    }

    if (m_Enclosing) {
        m_Enclosing->assign(identifier, value);
        return;
    }

    throw RuntimeError(identifier, "Undefined variable '" + identifier.lexeme + "'.");
}

void Environment::assignAt(int distance, const Token& identifier, const Object& value) {
    ancestor(distance)->m_Values[identifier.lexeme] = value;
}

Environment* Environment::ancestor(int distance) {
    auto environment = this;
    for (int i = 0; i < distance; ++i) {
        if (!environment->m_Enclosing)
            break; // throw RuntimeError; ????????

        environment = environment->m_Enclosing.get();
    }

    return environment;
}
