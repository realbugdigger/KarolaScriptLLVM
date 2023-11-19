#pragma once

#include <utility>

#include "KarolaScriptCallable.h"
#include "KarolaScriptInstance.h"
#include "../parser/Expr.h"
#include "../parser/Stmt.h"

class KarolaScriptFunction : public KarolaScriptCallable {
private:
    std::shared_ptr<Function> m_Declaration;
    std::shared_ptr<Environment> m_Closure;
    bool m_IsInitializer;
public:
    KarolaScriptFunction(std::shared_ptr<Function>& declaration_, std::shared_ptr<Environment>& closure_, bool isInitializer_)
        : m_Declaration(std::move(declaration_)), m_Closure(std::move(closure_)), m_IsInitializer(isInitializer_)
        {}

    int arity() override {
        return m_Declaration->m_Params.size();
    }

    // params should be passed and declared inside executeBlock() method, this shouldn't happen probably
    // funct scope(a) {
    //      var a = "local";
    // }
    std::shared_ptr<std::any> call(const std::shared_ptr<Interpreter>& interpreter, const std::vector<std::shared_ptr<std::any>>& arguments) override {
        std::shared_ptr<Environment> environment(new Environment(m_Closure));

        if (!arguments.empty()) {
            for (int i = 0; i < m_Declaration->m_Params.size(); i++) {
                if (arguments[i] && arguments[i]->type() == typeid(AnonFunction)) {
                    try {
                        std::shared_ptr<Function> stmt = std::make_shared<Function>(m_Declaration->m_Params[i], std::any_cast<AnonFunction>(arguments[i]).m_Params, std::any_cast<AnonFunction>(arguments[i]).m_Body);
                        std::shared_ptr<KarolaScriptFunction> function = std::make_shared<KarolaScriptFunction>(stmt, environment, false);
                        environment->define(m_Declaration->m_Params[i].lexeme, function);
                    }
                    catch(const std::bad_any_cast& e) {
                        std::cout << "Bad cast: " << e.what() << std::endl; // This should definitely never ever happen.
                    }
                } else {
                    environment->define(m_Declaration->m_Params[i].lexeme, arguments[i]);
                }
            }
        }

        try {
            interpreter->executeBlock(m_Declaration->m_Body, environment);
        } catch (ReturnException& returnValue) {
            if (m_IsInitializer) {
                return m_Closure->getAt(0, "this");
            }
            return returnValue.m_Value;
        }

        if (m_IsInitializer) {
            return m_Closure->getAt(0, "this");
        }
        return nullptr;
    }

    std::string toString() {
        return "<fn " + m_Declaration->m_Name.lexeme + ">";
    }

    std::shared_ptr<KarolaScriptFunction> bind(const std::shared_ptr<KarolaScriptInstance>& instance) {
        std::shared_ptr<Environment> environment = std::make_shared<Environment>(m_Closure);
        environment->define("this", instance);
        return std::make_shared<KarolaScriptFunction>(m_Declaration, environment, m_IsInitializer);
    }
};