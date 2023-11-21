#include "Object.h"

#include <cmath>
#include <stdexcept>
#include <utility>

#include "../interpreter/KarolaScriptCallable.h"
#include "../interpreter/KarolaScriptClass.h"
#include "../lexer/Token.h"


Object::Object(const Token &token) {
    switch (token.type) {
        case TOKEN_NUMBER:
            type = ObjType::OBJTYPE_NUMBER;
            number = std::stod(token.lexeme);
            break;
        case TOKEN_TRUE:
            type = ObjType::OBJTYPE_BOOL;
            boolean = true;
            break;
        case TOKEN_FALSE:
            type = ObjType::OBJTYPE_BOOL;
            boolean = false;
            break;
        case TOKEN_STRING:
            type = ObjType::OBJTYPE_STRING;
            str = token.lexeme;
            break;
        case TOKEN_NULL:
            type = ObjType::OBJTYPE_NULL;
            break;
        default:
            throw std::runtime_error("Invalid token type when constructing LoxObject");
    }
}

Object::Object(double number) : type(ObjType::OBJTYPE_NUMBER), number(number) {}

Object::Object(const std::string &string) : type(ObjType::OBJTYPE_STRING), str(string) {}

Object::Object(const char* string) : Object(std::string(string)) {}

Object::Object(bool boolean) : type(ObjType::OBJTYPE_BOOL), boolean(boolean) {}

Object::Object(SharedCallablePtr callable) : type(ObjType::OBJTYPE_CALLABLE), callable(std::move(callable)) {}

Object::Object(SharedInstancePtr instance) : type(ObjType::OBJTYPE_INSTANCE), instance(std::move(instance)) {}

Object Object::Null() {
    return Object();
}

Object::Object() : type(ObjType::OBJTYPE_NULL) {} //Initializes the object as NULL

bool Object::isNumber() const {
    return type == ObjType::OBJTYPE_NUMBER;
}

bool Object::isBoolean() const {
    return type == ObjType::OBJTYPE_BOOL;
}

bool Object::isString() const {
    return type == ObjType::OBJTYPE_STRING;
}

bool Object::isNull() const {
    return type == ObjType::OBJTYPE_NULL;
}

bool Object::isCallable() const {
    return type == ObjType::OBJTYPE_CALLABLE;
}

bool Object::isInstance() const {
    return type == ObjType::OBJTYPE_INSTANCE;
}

double Object::getNumber() const {
    if (!isNumber()){
        throw std::runtime_error("Object does not contain a number");
    }
    return number;
}

bool Object::getBoolean() const {
    if (!isBoolean()){
        throw std::runtime_error("Object does not contain a boolean");
    }
    return boolean;
}

std::string Object::getString() const {
    if (!isString()){
        throw std::runtime_error("Object does not contain a string");
    }
    return str;
}

SharedCallablePtr Object::getCallable() const {
    if (!isCallable()){
        throw std::runtime_error("Object does not contain a callable");
    }
    return callable;
}

SharedInstancePtr Object::getClassInstance() const {
    if (!isInstance()){
        throw std::runtime_error("Object does not contain a class instance");
    }
    return instance;
}