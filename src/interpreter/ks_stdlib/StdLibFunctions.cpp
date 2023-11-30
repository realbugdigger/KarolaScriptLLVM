#include "StdLibFunctions.h"
#include <chrono>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <cctype>
#include <string>
#include <memory>
#include <stdexcept>
#include <thread>
#include <sstream>
#include "../RuntimeError.h"
#include "../../lexer/lexer.h"

class Interpreter;

stdlibFunctions::Clock::Clock() : KarolaScriptCallable(CallableType::FUNCTION) {}

Object stdlibFunctions::Clock::call(Interpreter &interpreter, const std::vector<Object> &arguments) {
    using namespace std::chrono;
    double ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    std::cout << ms << std::endl;
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

stdlibFunctions::Input::Input() : KarolaScriptCallable(CallableType::FUNCTION) {}

Object stdlibFunctions::Input::call(Interpreter &interpreter, const std::vector<Object> &arguments) {
    std::string input;
    std::getline(std::cin, input);
    return Object(input);
}

int stdlibFunctions::Input::arity() {
    return 0;
}

std::string stdlibFunctions::Input::toString() {
    return "<native function " + name() + ">";
}

std::string stdlibFunctions::Input::name() {
    return "input";
}

stdlibFunctions::ToUpper::ToUpper() : KarolaScriptCallable(CallableType::FUNCTION) {}

Object stdlibFunctions::ToUpper::call(Interpreter &interpreter, const std::vector<Object> &arguments) {
    if (!arguments[0].isString())
        throw RuntimeError("toLower argument should be a string.");

    std::string s = arguments[0].getString();

    for (char& c : s) {
        c = std::toupper(c);
    }

    return Object(s);
}

int stdlibFunctions::ToUpper::arity() {
    return 1;
}

std::string stdlibFunctions::ToUpper::toString() {
    return "<native function " + name() + ">";
}

std::string stdlibFunctions::ToUpper::name() {
    return "toUpper";
}

stdlibFunctions::ToLower::ToLower() : KarolaScriptCallable(CallableType::FUNCTION) {}

Object stdlibFunctions::ToLower::call(Interpreter &interpreter, const std::vector<Object> &arguments) {
    if (!arguments[0].isString())
        throw RuntimeError("toLower argument should be a string.");

    std::string s = arguments[0].getString();

    for (char& c : s) {
        c = std::tolower(c);
    }

    return Object(s);
}

int stdlibFunctions::ToLower::arity() {
    return 1;
}

std::string stdlibFunctions::ToLower::toString() {
    return "<native function " + name() + ">";
}

std::string stdlibFunctions::ToLower::name() {
    return "toLower";
}

stdlibFunctions::Power::Power() : KarolaScriptCallable(CallableType::FUNCTION) {}

Object stdlibFunctions::Power::call(Interpreter &interpreter, const std::vector<Object> &arguments) {
    if (!arguments[0].isNumber() || !arguments[1].isNumber())
        throw RuntimeError("Both pwr argument should be a number.");

    double number = arguments[0].getNumber();
    double pwr = arguments[1].getNumber();

    return Object(pow(number, pwr));
}

int stdlibFunctions::Power::arity() {
    return 2;
}

std::string stdlibFunctions::Power::toString() {
    return "<native function " + name() + ">";
}

std::string stdlibFunctions::Power::name() {
    return "pwr";
}

stdlibFunctions::SqrRoot::SqrRoot() : KarolaScriptCallable(CallableType::FUNCTION) {}

Object stdlibFunctions::SqrRoot::call(Interpreter &interpreter, const std::vector<Object> &arguments) {
    if (!arguments[0].isNumber())
        throw RuntimeError("toLower argument should be a string.");

    return Object(sqrt(arguments[0].getNumber()));
}

int stdlibFunctions::SqrRoot::arity() {
    return 1;
}

std::string stdlibFunctions::SqrRoot::toString() {
    return "<native function " + name() + ">";
}

std::string stdlibFunctions::SqrRoot::name() {
    return "sqrr00t";
}