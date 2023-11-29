#include "KarolaScriptClass.h"

#include <iostream>
#include <utility>
#include <sstream>
#include <cassert>

#include "RuntimeError.h"
#include "../util/Object.h"
#include "../lexer/Token.h"
#include "KarolaScriptFunction.h"

KarolaScriptClass::KarolaScriptClass(const std::string& name_,
                                    const std::optional<SharedCallablePtr> superclass_,
                                    const std::unordered_map<std::string, Object>& methods_,
                                    const std::unordered_map<std::string, Object>& staticMethods_
                                    ) : KarolaScriptCallable(CallableType::CLASS), m_ClassName(name_), m_Superclass(superclass_), m_Methods(methods_), m_StaticMethods(staticMethods_)
{
    if (m_Superclass.has_value() && m_Superclass->get()->m_Type != CallableType::CLASS)
        throw std::runtime_error("Class can only inherit a class.");
}

Object KarolaScriptClass::call(Interpreter& interpreter, const std::vector<Object>& arguments) {
    SharedInstancePtr instance = std::make_shared<KarolaScriptInstance>(shared_from_this());
    std::optional<Object> initializer = findMethod("init");
    if (initializer.has_value()) {
    //            initializer->bind(instance)->call(interpreter, arguments);
        KarolaScriptFunction* function = dynamic_cast<KarolaScriptFunction*>(initializer.value().getCallable().get());

        //Create a new version of the constructor with "this" bound
        KarolaScriptFunction* newFunction = function->bind(instance);
        //call the constructor
        newFunction->call(interpreter, arguments);
        //Once we call the constructor we no longer need the newFunction so delete it before it leaks.
        delete newFunction;
    }
    Object instanceObj(instance);
    return instanceObj;
}

std::optional<Object> KarolaScriptClass::findMethod(const std::string& name) {
    if (m_Methods.find(name) != m_Methods.end()) {
        return m_Methods[name];
    }

    if (m_Superclass.has_value()) {
        auto* ksClass = dynamic_cast<KarolaScriptClass*>(m_Superclass.value().get());
        return ksClass->findMethod(name);
    }

    return std::nullopt;
}

//std::optional<Object> KarolaScriptClass::findMethod(KarolaScriptInstance& instance, const std::string& name) {
//    if (m_Methods.find(name) != m_Methods.end()) {
//        Object method = m_Methods[name];
//        KarolaScriptFunction *function = dynamic_cast<KarolaScriptFunction*>(method.getCallable().get());
//        //Create a new function where the variable "this" is binded to this instance
//        SharedCallablePtr newFunction(function->bind(shared_from_this()));
//        Object newFunctionObject(newFunction);
//        return newFunctionObject;
//    }
//
//    if (m_Superclass.has_value()) {
//        auto* ksClass = dynamic_cast<KarolaScriptClass*>(m_Superclass.value().get());
//        return ksClass->findMethod(instance, name);
//    }
//
//    return std::nullopt;
//}

std::optional<Object> KarolaScriptClass::findStaticMethod(const std::string& name) {
    if (m_StaticMethods.find(name) != m_StaticMethods.end()) {
            return m_StaticMethods.at(name);
    }
    return Object::Null();
}

Object KarolaScriptClass::getProperty(const Token& identifier) {
    return findStaticMethod(identifier.lexeme).value();
}

int KarolaScriptClass::arity() {
    std::optional<Object> initializer = findMethod("init");
    if (!initializer.has_value()) return 0;

    return initializer->getCallable()->arity();
}

std::string KarolaScriptClass::toString() {
    return "<class " + name() + ">";
}

std::string KarolaScriptClass::name() {
    return m_ClassName;
}


KarolaScriptInstance::KarolaScriptInstance(std::shared_ptr<KarolaScriptClass> klass_) : m_Klass(std::move(klass_)) {}

Object KarolaScriptInstance::getProperty(const Token& identifier) {
    auto searched = m_Fields.find(identifier.lexeme);
    if (searched != m_Fields.end()) {
        return searched->second;
    }

    std::optional<Object> method = m_Klass->findMethod(identifier.lexeme);
    if (method.has_value()) {
        KarolaScriptFunction *function = dynamic_cast<KarolaScriptFunction*>(method.value().getCallable().get());
        //Create a new function where the variable "this" is binded to this instance
        SharedCallablePtr newFunction(function->bind(shared_from_this()));
        Object newFunctionObject(newFunction);
        return newFunctionObject;
    }

    throw RuntimeError(identifier, "Undefined property '" + identifier.lexeme + "'.");
}

void KarolaScriptInstance::setProperty(const Token& identifier, const Object& value) {
    m_Fields[identifier.lexeme] = value;
}

std::string KarolaScriptInstance::toString() {
    std::stringstream ss;
    ss << "<Instance of class " << m_Klass->name() << " at " << this << ">";
    return ss.str();
//        return m_Klass.m_ClassName + " instance";
}