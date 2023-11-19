#pragma once

#include <unordered_map>
#include <memory>
#include <any>

#include "../lexer/Token.h"
#include "RuntimeError.h"

class Environment {
public:
    std::shared_ptr<Environment> enclosing;
    std::unordered_map<std::string, std::shared_ptr<std::any>> values;
public:
    Environment() = default;

    explicit Environment(std::shared_ptr<Environment>& enclosing)
            : enclosing{std::move(enclosing)} {
    }

    void define(const std::string& identifier, const std::any& value) {
        // Define a new identifier.
        values.try_emplace(identifier, std::make_shared<std::any>(value));
    }

//    void define(const std::string& identifier, const std::shared_ptr<std::any>& ptr_to_val) {
//        // Define a new identifier.
//        values.try_emplace(identifier, ptr_to_val);
//    }

    // `lookup()` can be renamed to `get()`
    std::shared_ptr<std::any> lookup(const Token& identifier) {
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

    void assign(const Token& identifier, const std::any& value) {
        if (values.find(identifier.lexeme) != values.end()) {
            *(values[identifier.lexeme]) = value;
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
        for (int i = 0; i < distance; ++i)
        {
            if (!environment->enclosing)
                break;

            environment = environment->enclosing.get();
        }

        return environment;
    }

    std::shared_ptr<std::any> getAt(int distance, const std::string& identifier) {
        return ancestor(distance)->values[identifier];
    }

    void assignAt(int distance, const Token& identifier, const std::any& value) {
        *(ancestor(distance)->values[identifier.lexeme]) = value;
    }
};