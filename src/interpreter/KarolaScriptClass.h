#pragma once

#include <map>
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

class KarolaScriptClass : public KarolaScriptCallable {
public:
    std::string m_Name;
    std::shared_ptr<KarolaScriptClass> m_Superclass;
    std::unordered_map<std::string, std::shared_ptr<KarolaScriptFunction>> m_Methods;
public:
    KarolaScriptClass(std::string& name_, const std::shared_ptr<KarolaScriptClass>& superclass_, const std::unordered_map<std::string, std::shared_ptr<KarolaScriptFunction>>& methods_)
        : m_Name(std::move(name_)), m_Superclass(superclass_), m_Methods(methods_) {}

    Object call(std::shared_ptr<Interpreter> interpreter, std::vector<Object> arguments) override {
        auto instance = std::shared_ptr<LoxInstance>(new LoxInstance(*this));
        std::shared_ptr<KarolaScriptFunction> initializer = findMethod("init");
        if (initializer != nullptr) {
            initializer->bind(instance)->call(interpreter, arguments);
        }
        return Object::make_instance_obj(instance);
    }

    int arity() override {
        std::shared_ptr<KarolaScriptFunction> initializer = findMethod("init");
        if (initializer == nullptr) return 0;
        return initializer->arity();
    }

    std::string toString() {
        return m_Name;
    }

    std::shared_ptr<KarolaScriptFunction> findMethod(const std::string& name) {
        auto searched = m_Methods.find(name);
        if (searched != m_Methods.end()) {
            return searched->second;
        }
        if (m_Superclass != nullptr) {
            return m_Superclass->findMethod(name);
        }
        return 0;
    }
};