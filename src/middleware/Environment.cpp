#include "Environment.h"

Environment::Environment(std::shared_ptr<Environment> enclosing) : m_Enclosing{std::move(enclosing)} {}

template <typename K, typename V> void Environment::define(K& identifier, V* value) {
    const std::string& key = identifier;
    if (m_Values.count(key) == 1){
//        throw RuntimeError("Cannot redefine a variable. Variable '" + key + "' has already been defined");
    }

    m_Values[key] = value;
}

template <typename K, typename V> V* Environment::lookup(K& identifier) {
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
//    throw RuntimeError("Undefined variable '" + identifier + "'.");
}

template <typename K, typename V> V* Environment::getAt(int distance, K& identifier) {
    return ancestor(distance)->m_Values[identifier];
}

template <typename K, typename V> void Environment::assign(K& identifier, V* value) {
    if (m_Values.find(identifier) != m_Values.end()) {
        m_Values[identifier] = value;
        return;
    }

    if (m_Enclosing) {
        m_Enclosing->assign(identifier, value);
        return;
    }

    throw RuntimeError("Undefined variable '" + identifier + "'.");
}

template <typename K, typename V> void Environment::assignAt(int distance, K& identifier, V* value) {
    ancestor(distance)->m_Values[identifier] = value;
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
