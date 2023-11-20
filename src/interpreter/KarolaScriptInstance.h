#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <memory>
#include <any>
#include <utility>
#include <sstream>

#include "../lexer/Token.h"
#include "../util/Object.h"
#include "RuntimeError.h"
#include "KarolaScriptClass.h"

class KarolaScriptInstance : public std::enable_shared_from_this<KarolaScriptInstance> {
private:
    std::shared_ptr<KarolaScriptClass> m_Klass;
    std::unordered_map<std::string, Object> fields;
public:
    explicit KarolaScriptInstance(std::shared_ptr<KarolaScriptClass> klass_) : m_Klass(std::move(klass_)) {}

//    explicit KarolaScriptInstance(const std::shared_ptr<KarolaScriptClass>& klass_): m_Klass(klass_) {}

    virtual std::string toString() {
        std::stringstream ss;
        ss << "<Instance of class " << m_Klass->name() << " at " << this << ">";
        return ss.str();
//        return m_Klass.m_ClassName + " instance";
    }

    virtual Object getProperty(const Token& identifier) {
        auto searched = fields.find(identifier.lexeme);
        if (searched != fields.end()) {
            return searched->second;
        }

        std::shared_ptr<KarolaScriptFunction> method = m_Klass->findMethod(shared_from_this(), identifier.lexeme);
        if (method != nullptr) {
            std::shared_ptr<KarolaScriptFunction> newFunction = method->bind(shared_from_this());
            Object newFunctionObject(newFunction);
            return newFunctionObject;
        }

        throw RuntimeError(identifier, "Undefined property '" + identifier.lexeme + "'.");
    }

    void setProperty(const Token& identifier, const Object& value) {
        fields[identifier.lexeme] = value;
    }
};