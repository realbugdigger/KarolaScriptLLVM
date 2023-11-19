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
#include "Interpreter.h"

class KarolaScriptClass : public KarolaScriptInstance, public KarolaScriptCallable {
public:
    std::string m_Name;
    std::shared_ptr<KarolaScriptClass> m_Superclass;
    std::unordered_map<std::string, std::shared_ptr<KarolaScriptFunction>> m_Methods;
    std::unordered_map<std::string, std::shared_ptr<KarolaScriptFunction>> m_ClassMethods;
public:
    KarolaScriptClass(std::string& name_, const std::shared_ptr<KarolaScriptClass>& superclass_,
                      const std::unordered_map<std::string, std::shared_ptr<KarolaScriptFunction>>& methods_, const std::unordered_map<std::string, std::shared_ptr<KarolaScriptFunction>>& classMethods_)
        : m_Name(std::move(name_)), m_Superclass(superclass_), m_Methods(methods_), m_ClassMethods(classMethods_) {}

    std::shared_ptr<std::any> call(const std::shared_ptr<Interpreter>& interpreter, const std::vector<std::shared_ptr<std::any>>& arguments) override {
//        auto instance2 = std::make_shared<KarolaScriptInstance>(std::move(*this));
//        auto instance1 = std::shared_ptr<KarolaScriptInstance>(new KarolaScriptInstance(*this));
        auto instance = std::make_shared<KarolaScriptInstance>(*this);
        std::shared_ptr<KarolaScriptFunction> initializer = findMethod(instance, "init");
        if (initializer != nullptr) {
            initializer->bind(instance)->call(interpreter, arguments);
        }
        return instance;
    }

    int arity() override {
        std::shared_ptr<KarolaScriptFunction> initializer = findMethod(nullptr, "init");
        if (initializer == nullptr) return 0;

        return initializer->arity();
    }

    std::string toString() override {
        return m_Name;
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

    std::any get(const Token& name) override {
        std::shared_ptr<KarolaScriptFunction> method = findClassMethod(name.lexeme);
        return method;
    }
};