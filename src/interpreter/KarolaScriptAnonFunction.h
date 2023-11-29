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

    Object call(Interpreter& interpreter, const std::vector<Object>& arguments) override;
    int arity() override;
    std::string toString() override {return "";}
    std::string name() override {return "";}
};