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

    class Input : public KarolaScriptCallable {
    public:
        Input();
        Object call(Interpreter &interpreter, const std::vector<Object> &arguments) override;
        int arity() override;
        std::string toString() override;
        std::string name() override;
    };

    class ToUpper : public KarolaScriptCallable {
    public:
        ToUpper();
        Object call(Interpreter &interpreter, const std::vector<Object> &arguments) override;
        int arity() override;
        std::string toString() override;
        std::string name() override;
    };

    class ToLower : public KarolaScriptCallable {
    public:
        ToLower();
        Object call(Interpreter &interpreter, const std::vector<Object> &arguments) override;
        int arity() override;
        std::string toString() override;
        std::string name() override;
    };


    // static methods of Math clazz

    class Power : public KarolaScriptCallable {
    public:
        Power();
        Object call(Interpreter &interpreter, const std::vector<Object> &arguments) override;
        int arity() override;
        std::string toString() override;
        std::string name() override;
    };

    class SqrRoot : public KarolaScriptCallable {
    public:
        SqrRoot();
        Object call(Interpreter &interpreter, const std::vector<Object> &arguments) override;
        int arity() override;
        std::string toString() override;
        std::string name() override;
    };
}
