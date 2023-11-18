#pragma once

#include <vector>
#include <memory>
#include "../lexer/Token.h"
#include "Interpreter.h"

class KarolaScriptCallable {
public:
    virtual ~KarolaScriptCallable() = default;  // for derived class
    virtual int arity() = 0;
    virtual Object call(std::shared_ptr<Interpreter> interpreter, std::vector<Object> arguments) = 0;
};