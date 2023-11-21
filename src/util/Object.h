#pragma once

#include "../interpreter/KarolaScriptInstance.h"
#include "../interpreter/KarolaScriptFunction.h"
#include "../interpreter/KarolaScriptCallable.h"


enum ObjType {
    OBJTYPE_NULL, OBJTYPE_BOOL, OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_CALLABLE, OBJTYPE_CLASS, OBJTYPE_FUNCTION, OBJTYPE_INSTANCE
};

/* A shared ptr is used because resources such as functions, classes, and instances are created and stored in memory only once but can
 * be shared with multiple users. For example, two variables can refer to the same function.
 * */
using SharedCallablePtr = std::shared_ptr<KarolaScriptCallable>;
using SharedInstancePtr = std::shared_ptr<KarolaScriptInstance>;

/* Object class is used to represent variables, instances, functions, classes, etc, essentially surrendering type safety
 * and having to depend on instanceof checks. I attempted to maintain some type safety with this class.
 * LoxObject is a wrapper that can hold literals, callables
 * such as functions and classes, and instances.
 * */
class Object {
private:
    double number = 0.0;
    bool boolean = false;
    std::string str;
    SharedCallablePtr callable;
    SharedInstancePtr instance;
    ObjType type = ObjType::OBJTYPE_NULL;

public:
    explicit Object(const Token &token) {
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

    explicit Object(double number) : type(ObjType::OBJTYPE_NUMBER), number(number) {}

    explicit Object(const std::string &string) : type(ObjType::OBJTYPE_STRING), str(string) {}

    explicit Object(const char* string) : Object(std::string(string)) {}

    explicit Object(bool boolean) : type(ObjType::OBJTYPE_BOOL), boolean(boolean) {}

    explicit Object(SharedCallablePtr callable) : type(ObjType::OBJTYPE_CALLABLE), callable(std::move(callable)) {}

    explicit Object(KarolaScriptCallable* ptr) = delete;

    explicit Object(SharedInstancePtr instance) : type(ObjType::OBJTYPE_INSTANCE), instance(std::move(instance)) {}

    explicit Object(KarolaScriptInstance* ptr) = delete;

    static Object Null() {
        return Object();
    }

    Object() : type(ObjType::OBJTYPE_NULL) {} //Initializes the object as NULL

    bool isNumber() const {
        return type == ObjType::OBJTYPE_NUMBER;
    }

    bool isBoolean() const {
        return type == ObjType::OBJTYPE_BOOL;
    }

    bool isString() const {
        return type == ObjType::OBJTYPE_STRING;
    }

    bool isNull() const {
        return type == ObjType::OBJTYPE_NULL;
    }

    bool isCallable() const {
        return type == ObjType::OBJTYPE_CALLABLE;
    }

    bool isInstance() const {
        return type == ObjType::OBJTYPE_INSTANCE;
    }

    double getNumber() const {
        if (!isNumber()){
            throw std::runtime_error("Object does not contain a number");
        }
        return number;
    }

    bool getBoolean() const {
        if (!isBoolean()){
            throw std::runtime_error("Object does not contain a boolean");
        }
        return boolean;
    }

    std::string getString() const {
        if (!isString()){
            throw std::runtime_error("Object does not contain a string");
        }
        return str;
    }

    SharedCallablePtr getCallable() const {
        if (!isCallable()){
            throw std::runtime_error("Object does not contain a callable");
        }
        return callable;
    }

    SharedInstancePtr getClassInstance() const {
        if (!isInstance()){
            throw std::runtime_error("Object does not contain a class instance");
        }
        return instance;
    }
};