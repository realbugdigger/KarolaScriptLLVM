#include "StdLibFunctions.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <thread>
#include <sstream>
#include "../RuntimeError.h"

class Interpreter;

stdlibFunctions::Clock::Clock() : KarolaScriptCallable(CallableType::FUNCTION) {}

Object stdlibFunctions::Clock::call(Interpreter &interpreter, const std::vector<Object> &arguments) {
    using namespace std::chrono;
    double ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    return Object(ms);
}

int stdlibFunctions::Clock::arity() {
    return 0;
}

std::string stdlibFunctions::Clock::toString() {
    return "<native function " + name() + ">";
}

std::string stdlibFunctions::Clock::name() {
    return "clock";
}

stdlibFunctions::Sleep::Sleep() : KarolaScriptCallable(CallableType::FUNCTION) {}

Object stdlibFunctions::Sleep::call(Interpreter &interpreter, const std::vector<Object> &arguments) {
    int time;
    try {
        time = (int) arguments[0].getNumber();
    } catch (const std::runtime_error &error) {
        throw RuntimeError("Function 'sleep' expected an integer as its argument");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(time));
    return Object::Null();
}

int stdlibFunctions::Sleep::arity() {
    return 1;
}

std::string stdlibFunctions::Sleep::toString() {
    return "<native function " + name() + ">";
}

std::string stdlibFunctions::Sleep::name() {
    return "sleep";
}
