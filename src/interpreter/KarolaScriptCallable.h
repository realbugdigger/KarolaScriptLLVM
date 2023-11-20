#pragma once

#include <vector>
#include <memory>

#include "../lexer/Token.h"
#include "../util/Object.h"
#include "Interpreter.h"

class KarolaScriptCallable {
public:
    virtual ~KarolaScriptCallable() = default;  // for derived class
    virtual int arity() = 0;
    virtual Object call(const std::shared_ptr<Interpreter>& interpreter, const std::vector<Object>& arguments) = 0;
};