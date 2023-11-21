#pragma once

#include <vector>
#include <memory>
#include <string>

class Object;
class Interpreter;

class KarolaScriptCallable {
public:
    enum CallableType {
        FUNCTION, CLASS, ANON_FUNCTION
    };

    CallableType m_Type;

    explicit KarolaScriptCallable(CallableType type) : m_Type(type) {};
    virtual ~KarolaScriptCallable() = default;  // for derived class

    virtual Object call(Interpreter& interpreter, const std::vector<Object>& arguments) = 0;
    virtual int arity() = 0;
    virtual std::string toString() = 0;
    virtual std::string name() = 0;
};