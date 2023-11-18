#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <memory>
#include <any>
#include <utility>

#include "../lexer/Token.h"
#include "RuntimeError.h"
#include "KarolaScriptClass.h"

class KarolaScriptInstance {
private:
    KarolaScriptClass klass;
    std::unordered_map<std::string, std::any> fields;
public:
    explicit KarolaScriptInstance(KarolaScriptClass& klass_): klass(std::move(klass_)) {}

    std::string toString() {
        return klass.m_Name + " instance";
    }

    std::any get(const Token& name) {
        auto searched = fields.find(name.lexeme);
        if (searched != fields.end()) {
            return searched->second;
        }

        std::shared_ptr<KarolaScriptFunction> method = klass.findMethod(name.lexeme);
        if (method != nullptr) {
            return Object::make_fun_obj(method->bind(*this));
        }

        throw RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
    }

    void set(const Token& name, const std::any& value) {
        fields[name.lexeme] = std::move(value);
    }
};