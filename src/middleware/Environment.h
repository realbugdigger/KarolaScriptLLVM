#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <map>

#include <llvm/ADT/DenseMap.h>

template <typename K, typename V>
class Environment {
public:
    std::shared_ptr<Environment<K, V> > m_Enclosing;
    llvm::DenseMap<K, V> m_Values;
public:
    Environment() = default;

    explicit Environment(std::shared_ptr<Environment> enclosing);

    // Use the Token overload because it can then report errors using the token's line. Only use the string overload when there's no token.
    void define(K& identifier, V* value);

    // `lookup()` can be renamed to `get()`
    V* lookup(K& identifier);
    V* getAt(int distance, K& identifier);

    void assign(K& identifier, V* value);
    void assignAt(int distance, K& identifier, V* value);

    Environment* ancestor(int distance);
};