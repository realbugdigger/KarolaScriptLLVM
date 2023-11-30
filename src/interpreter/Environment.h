#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "../util/Object.h"

class Environment {
public:
    std::shared_ptr<Environment> m_Enclosing;
    std::unordered_map<std::string, Object> m_Values;
public:
    Environment() = default;

    explicit Environment(std::shared_ptr<Environment> enclosing);

    // Use the Token overload because it can then report errors using the token's line. Only use the string overload when there's no token.
    void define(const Token& identifier, const Object& value);
    void define(const std::string &key, const Object& value);

    // `lookup()` can be renamed to `get()`
    Object lookup(const Token& identifier);
    Object lookup(const std::string& identifier);
    Object getAt(int distance, const std::string& identifier);

    void assign(const Token& identifier, const Object& value);
    void assign(const std::string& identifier, const Object& value);
    void assignAt(int distance, const Token& identifier, const Object& value);

    Environment* ancestor(int distance);
};