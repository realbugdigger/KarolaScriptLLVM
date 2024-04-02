#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <map>

#include "llvm/IR/Value.h"

class Environment {
public:
    std::shared_ptr<Environment> m_Enclosing;
    std::unordered_map<std::string, llvm::Value*> m_Values;
public:
    Environment() = default;

    explicit Environment(std::shared_ptr<Environment> enclosing);

    // Use the Token overload because it can then report errors using the token's line. Only use the string overload when there's no token.
    void define(const std::string& identifier, llvm::Value* value);

    // `lookup()` can be renamed to `get()`
    llvm::Value* lookup(const std::string& identifier);
    llvm::Value* getAt(int distance, const std::string& identifier);

    void assign(const std::string& identifier, llvm::Value* value);
    void assignAt(int distance, const std::string& identifier, llvm::Value* value);

    Environment* ancestor(int distance);
};