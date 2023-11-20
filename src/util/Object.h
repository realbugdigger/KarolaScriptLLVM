#pragma once

#include "../interpreter/KarolaScriptInstance.h"
#include "../interpreter/KarolaScriptFunction.h"
#include "../interpreter/KarolaScriptCallable.h"


enum ObjType {
    OBJTYPE_NULL, OBJTYPE_BOOL, OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_CALLABLE, OBJTYPE_CLASS, OBJTYPE_FUNCTION, OBJTYPE_INSTANCE
};

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
    std::shared_ptr<KarolaScriptCallable> callable;
    std::shared_ptr<KarolaScriptInstance> instance;
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

    explicit Object(std::shared_ptr<KarolaScriptCallable> callable) : type(ObjType::OBJTYPE_CALLABLE), callable(std::move(callable)) {}

    explicit Object(KarolaScriptCallable* ptr) = delete;

    explicit Object(std::shared_ptr<KarolaScriptInstance> instance) : type(ObjType::OBJTYPE_INSTANCE), instance(std::move(instance)) {}

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
};