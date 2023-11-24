#pragma once

#include <string>
#include <vector>

#include "Environment.h"
#include "KarolaScriptCallable.h"
#include "../util/Object.h"

class Function;
class Interpreter;

class KarolaScriptFunction : public KarolaScriptCallable {
public:
    //non owning. All AST nodes are owned by runner.cpp
    const Function* m_Declaration;
    std::shared_ptr<Environment> m_Closure;
    bool m_IsInitializer_;
public:
    KarolaScriptFunction(const Function* declaration_, std::shared_ptr<Environment> closure_, bool isInitializer_ = false);

    // params should be passed and declared inside executeBlock() method, this shouldn't happen probably
    // funct scope(a) {
    //      var a = "local";
    // }
    Object call(Interpreter& interpreter, const std::vector<Object>& arguments) override;
    int arity() override;
    std::string toString() override;
    std::string name() override;

    //Creates a NEW function that is a copy of the current function but with a different closure where "this" is binded to an instance;
    KarolaScriptFunction* bind(SharedInstancePtr instance);
};