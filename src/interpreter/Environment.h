#pragma once

#include <unordered_map>
#include <memory>
#include <any>

#include "../lexer/Token.h"
#include "../util/Object.h"
#include "RuntimeError.h"

class Environment {
public:
    std::shared_ptr<Environment> enclosing;
    std::unordered_map<std::string, Object> values;
public:
    Environment() = default;

    explicit Environment(std::shared_ptr<Environment> enclosing)
            : enclosing{std::move(enclosing)} {}

    // Use the Token overload because it can then report errors using the token's line. Only use the string overload when there's no token.
    void define(const Token& identifier, const Object& value) {
        std::string key = identifier.lexeme;
        if (values.find(key) != values.end()){
            throw RuntimeError("Cannot redefine a variable. Variable '" + key + "' has already been defined", identifier.line);
        }

        values[key] = value;
    }

    void define(const std::string &key, const Object& value) {
        if (values.count(key) == 1){
            throw RuntimeError("Cannot redefine a variable. Variable '" + key + "' has already been defined");
        }

        values[key] = value;
    }

    // `lookup()` can be renamed to `get()`
    Object lookup(const Token& identifier) {
        // Check if the current environment contains the identifier.
        if (values.find(identifier.lexeme) != values.end()) {
            // If so, return the value associated with it.
            return values[identifier.lexeme];
        }

        // If the identifier is not in the current environment, check the parent environment until global scope.
        if (enclosing) {
            return enclosing->lookup(identifier);
        }

        // If not in global scope, throw error.
        throw RuntimeError(identifier, "Undefined variable '" + identifier.lexeme + "'.");
    }

    void assign(const Token& identifier, const Object& value) {
        if (values.find(identifier.lexeme) != values.end()) {
            values[identifier.lexeme] = value;
            return;
        }

        if (enclosing) {
            enclosing->assign(identifier, value);
            return;
        }

        throw RuntimeError(identifier, "Undefined variable '" + identifier.lexeme + "'.");
    }

    Environment* ancestor(int distance) {
        auto environment = this;
        for (int i = 0; i < distance; ++i) {
            if (!environment->enclosing)
                break; // throw RuntimeError; ????????

            environment = environment->enclosing.get();
        }

        return environment;
    }

    Object getAt(int distance, const std::string& identifier) {
        return ancestor(distance)->values[identifier];
    }

    void assignAt(int distance, const Token& identifier, const Object& value) {
        ancestor(distance)->values[identifier.lexeme] = value;
    }
};