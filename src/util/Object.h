#pragma once

#include <iosfwd>
#include <memory>
#include <string>

enum ObjType {
    OBJTYPE_NULL, OBJTYPE_BOOL, OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_CALLABLE, OBJTYPE_CLASS, OBJTYPE_ANONFUNCTION, OBJTYPE_FUNCTION, OBJTYPE_INSTANCE
};

class KarolaScriptCallable;
class KarolaScriptInstance;

class AnonFunction;

struct Token;

/* A shared ptr is used because resources such as functions, classes, and instances are created and stored in memory only once but can
 * be shared with multiple users. For example, two variables can refer to the same function.
 * */
using SharedCallablePtr = std::shared_ptr<KarolaScriptCallable>;
using SharedAnonFunctPtr = std::shared_ptr<AnonFunction>;
using SharedInstancePtr = std::shared_ptr<KarolaScriptInstance>;

/* Object class is used to represent variables, instances, functions, classes, etc, essentially surrendering type safety
 * and having to depend on instanceof checks. I attempted to maintain some type safety with this class.
 * Object is a wrapper that can hold literals, callables
 * such as functions and classes, and instances.
 * */
class Object {
private:
    double number = 0.0;
    bool boolean = false;
    std::string str;
    SharedCallablePtr callable;
    SharedAnonFunctPtr anonFunction;
    SharedInstancePtr instance;
public:
    ObjType type = ObjType::OBJTYPE_NULL;

    explicit Object(const Token &token);

    explicit Object(double number);

    explicit Object(const std::string &string);

    explicit Object(const char* string);

    explicit Object(bool boolean);

    explicit Object(SharedCallablePtr callable);

    explicit Object(KarolaScriptCallable* ptr) = delete;

    explicit Object(SharedAnonFunctPtr anonFunctPtr);

    explicit Object(AnonFunction* ptr) = delete;

    explicit Object(SharedInstancePtr instance);

    explicit Object(KarolaScriptInstance* ptr) = delete;

    static Object Null();

    Object(); //Initializes the object as NULL

    bool isNumber() const;

    bool isBoolean() const;

    bool isString() const;

    bool isNull() const;

    bool isCallable() const;

    bool isAnonFunction() const;

    bool isInstance() const;

    double getNumber() const;

    bool getBoolean() const;

    std::string getString() const;

    SharedCallablePtr getCallable() const;

    SharedAnonFunctPtr getAnonFunction() const;

    SharedInstancePtr getClassInstance() const;
};