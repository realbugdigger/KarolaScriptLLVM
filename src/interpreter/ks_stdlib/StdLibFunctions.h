#pragma once

#include <string>
#include <vector>
#include "../KarolaScriptCallable.h"
#include "../../util/Object.h"

class Interpreter;

namespace stdlibFunctions {

    class Clock : public KarolaScriptCallable {
    public:
        Clock();
        Object call(Interpreter &interpreter, const std::vector<Object> &arguments) override;
        int arity() override;
        std::string toString() override;
        std::string name() override;
    };

    class Sleep : public KarolaScriptCallable {
    public:
        Sleep();
        Object call(Interpreter &interpreter, const std::vector<Object> &arguments) override;
        int arity() override;
        std::string toString() override;
        std::string name() override;
    };

}
