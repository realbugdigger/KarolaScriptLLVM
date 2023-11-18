#pragma once

#include <vector>
#include <memory>
#include "../lexer/Token.h"
#include "Interpreter.h"

class KarolaScriptCallable {
public:
    virtual ~KarolaScriptCallable() = default;  // for derived class
    virtual int arity() = 0;
    virtual std::shared_ptr<std::any> call(const std::shared_ptr<Interpreter>& interpreter, const std::vector<std::shared_ptr<std::any>>& arguments) = 0;
};