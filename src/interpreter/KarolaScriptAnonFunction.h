#pragma once

#include <string>
#include <vector>

#include "Environment.h"
#include "KarolaScriptCallable.h"
#include "../util/Object.h"

class AnonFunction;
class Interpreter;

class KarolaScriptAnonFunction : public KarolaScriptCallable {
public:
    //non owning. All AST nodes are owned by runner.cpp
    const AnonFunction* m_Declaration;
    std::shared_ptr<Environment> m_Closure;
public:
    KarolaScriptAnonFunction(const AnonFunction* declaration_, std::shared_ptr<Environment> closure_);

    // params should be passed and declared inside executeBlock() method, this shouldn't happen probably
    // funct scope(a) {
    //      var a = "local";
    // }
    Object call(Interpreter& interpreter, const std::vector<Object>& arguments) override;
    int arity() override;
    std::string toString() override {}
    std::string name() override {}
};