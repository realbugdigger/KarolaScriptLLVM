#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include <string>
#include <memory>
#include <any>
#include <utility>
#include <vector>

#include "KarolaScriptInstance.h"
#include "KarolaScriptFunction.h"
#include "KarolaScriptCallable.h"
#include "../util/Object.h"
#include "Interpreter.h"

class KarolaScriptClass : public KarolaScriptCallable, public std::enable_shared_from_this<KarolaScriptClass> {
public:
    std::string m_ClassName;
    std::shared_ptr<KarolaScriptClass> m_Superclass;
    std::unordered_map<std::string, std::shared_ptr<KarolaScriptFunction>> m_Methods;
    std::unordered_map<std::string, std::shared_ptr<KarolaScriptFunction>> m_ClassMethods;
public:
    KarolaScriptClass(std::string& name_, const std::shared_ptr<KarolaScriptClass>& superclass_,
                      const std::unordered_map<std::string, std::shared_ptr<KarolaScriptFunction>>& methods_, const std::unordered_map<std::string, std::shared_ptr<KarolaScriptFunction>>& classMethods_)
        : m_ClassName(std::move(name_)), m_Superclass(superclass_), m_Methods(methods_), m_ClassMethods(classMethods_) {}

    Object call(const std::shared_ptr<Interpreter>& interpreter, const std::vector<Object>& arguments) override {
        std::shared_ptr<KarolaScriptInstance> instance = std::make_shared<KarolaScriptInstance>(shared_from_this());
        std::shared_ptr<KarolaScriptFunction> initializer = findMethod(instance, "init");
        if (initializer != nullptr) {
            initializer->bind(instance)->call(interpreter, arguments);

            //Create a new version of the constructor with "this" bound
//            std::shared_ptr<KarolaScriptFunction> newFunction = initializer->bind(instance);
            //call the constructor
//            newFunction->call(interpreter, arguments);
            //Once we call the constructor we no longer need the newFunction so delete it before it leaks.
//            delete newFunction;
        }
        Object instanceObj(instance);
        return instanceObj;
    }

    int arity() override {
        std::shared_ptr<KarolaScriptFunction> initializer = findMethod(nullptr, "init");
        if (initializer == nullptr) return 0;

        return initializer->arity();
    }

    std::string toString() {
        return "<class " + name() + ">";
    }

    std::shared_ptr<KarolaScriptFunction> findMethod(const std::shared_ptr<KarolaScriptInstance>& instance, const std::string& name) {
        auto searched = m_Methods.find(name);
        if (searched != m_Methods.end()) {
//            return searched->second;
            auto casted = std::static_pointer_cast<KarolaScriptFunction>(searched->second);
            return casted->bind(instance);
        }

        if (m_Superclass != nullptr) {
            return m_Superclass->findMethod(instance, name);
        }
        return nullptr;
    }

    std::shared_ptr<KarolaScriptFunction> findClassMethod(const std::string& name) {
        if (m_ClassMethods.find(name) != m_ClassMethods.end()) {
            return m_ClassMethods.at(name);
        }
        return nullptr;
    }

    std::any get(const Token& name) {
        std::shared_ptr<KarolaScriptFunction> method = findClassMethod(name.lexeme);
        return method;
    }

    std::string name() const {
        return m_ClassName;
    }
};