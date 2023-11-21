#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include <string>
#include <memory>
#include <any>
#include <utility>
#include <vector>
#include <optional>

#include "KarolaScriptInstance.h"
#include "KarolaScriptFunction.h"
#include "KarolaScriptCallable.h"
#include "../util/Object.h"
#include "Interpreter.h"

class KarolaScriptClass : public KarolaScriptCallable, public std::enable_shared_from_this<KarolaScriptClass> {
public:
    std::string m_ClassName;
    std::optional<SharedCallablePtr> m_Superclass;
    std::unordered_map<std::string, Object> m_Methods;
    std::unordered_map<std::string, Object> m_ClassMethods;
public:
    KarolaScriptClass(std::string& name_, const SharedCallablePtr& superclass_,
                      const std::unordered_map<std::string, Object>& methods_, const std::unordered_map<std::string, Object>& classMethods_)
        : m_ClassName(std::move(name_)), m_Superclass(superclass_), m_Methods(methods_), m_ClassMethods(classMethods_) {}

    Object call(const std::shared_ptr<Interpreter>& interpreter, const std::vector<Object>& arguments) override {
        SharedInstancePtr instance = std::make_shared<KarolaScriptInstance>(shared_from_this());
        std::optional<Object> initializer = findMethod("init");
        if (initializer.has_value()) {
//            initializer->bind(instance)->call(interpreter, arguments);

            KarolaScriptFunction *function = dynamic_cast<KarolaScriptFunction*>(initializer.value().getCallable().get());

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

    int arity() override {
        std::optional<Object> initializer = findMethod("init");
        if (!initializer.has_value()) return 0;

        return initializer->getCallable()->arity();
    }

    std::string toString() {
        return "<class " + name() + ">";
    }

    std::optional<Object> findMethod(const std::string& name) {
        if (m_Methods.find(name) != m_Methods.end()){
            return m_Methods[name];
        }

        if (m_Superclass.has_value()){
            auto* ksClass = dynamic_cast<KarolaScriptClass*>(m_Superclass.value().get());
            return ksClass->findMethod(name);
        }

        return std::nullopt;
    }

//    std::shared_ptr<KarolaScriptFunction> findClassMethod(const std::string& name) {
//        if (m_ClassMethods.find(name) != m_ClassMethods.end()) {
//            return m_ClassMethods.at(name);
//        }
//        return nullptr;
//    }

//    std::any get(const Token& name) {
//        std::shared_ptr<KarolaScriptFunction> method = findClassMethod(name.lexeme);
//        return method;
//    }

    std::string name() const {
        return m_ClassName;
    }
};