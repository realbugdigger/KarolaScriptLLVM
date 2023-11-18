#pragma once

#include <utility>

#include "KarolaScriptCallable.h"

class KarolaScriptFunction : public KarolaScriptCallable {
private:
    std::shared_ptr<Function> m_Declaration;
    std::shared_ptr<Environment> m_Closure;
    bool m_IsInitializer;
public:
    KarolaScriptFunction(std::shared_ptr<Function>& declaration_, std::shared_ptr<Environment>& closure_, bool isInitializer_ )
        : m_Declaration(std::move(declaration_)), m_Closure(std::move(closure_)), m_IsInitializer(isInitializer_)
        {}

    int arity() override {
        return m_Declaration->m_Params.size();
    }

    Object call(const std::shared_ptr<Interpreter>& interpreter, const std::vector<Object>& arguments) {
        std::shared_ptr<Environment> environment(new Environment(m_Closure));

        for (int i = 0; i < m_Declaration->m_Params.size(); i++) {
            environment->define(
                    m_Declaration->m_Params[i].lexeme,
                    arguments[i]);
        }

        try {
            interpreter->executeBlock(m_Declaration->m_Body, environment);
        } catch (ReturnException& returnValue) {
            if (m_IsInitializer) {
                return m_Closure->getAt(0, "this");
            }
            return returnValue.value;
        }
        if (m_IsInitializer) {
            return m_Closure->getAt(0, "this");
        }
        return Object::make_nil_obj();
    }

    std::string toString() {
        return "<fn " + m_Declaration->m_Name.lexeme + ">";
    }

    std::shared_ptr<KarolaScriptFunction> bind(std::shared_ptr<LoxInstance> instance) {
        std::shared_ptr<Environment> environment(new Environment(m_Closure));
        environment->define("this", Object::make_instance_obj(instance));
        return std::make_shared<KarolaScriptFunction>(m_Declaration, environment, m_IsInitializer);
    }
};