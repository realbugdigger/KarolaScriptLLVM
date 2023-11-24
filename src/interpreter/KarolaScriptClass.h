#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "KarolaScriptCallable.h"
#include "../util/Object.h"

class Interpreter;
struct Token;

class KarolaScriptClass : public KarolaScriptCallable, public std::enable_shared_from_this<KarolaScriptClass> {
public:
    std::string m_ClassName;
    std::optional<SharedCallablePtr> m_Superclass;
    std::unordered_map<std::string, Object> m_Methods;
    std::unordered_map<std::string, Object> m_ClassMethods;
public:
    KarolaScriptClass(const std::string& name_,
                      const std::optional<SharedCallablePtr> superclass_,
                      const std::unordered_map<std::string, Object>& methods_,
                      const std::unordered_map<std::string, Object>& classMethods_
                      );

    Object call(Interpreter& interpreter, const std::vector<Object>& arguments) override;
    std::optional<Object> findMethod(const std::string& name);
    int arity() override;
    std::string toString() override;
    std::string name() override;

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
};

class KarolaScriptInstance : public std::enable_shared_from_this<KarolaScriptInstance> {
private:
    std::shared_ptr<KarolaScriptClass> m_Klass;
    std::unordered_map<std::string, Object> m_Fields;
public:
    explicit KarolaScriptInstance(std::shared_ptr<KarolaScriptClass> klass_);
    //    explicit KarolaScriptInstance(const std::shared_ptr<KarolaScriptClass>& klass_);
    Object getProperty(const Token& identifier);
    void setProperty(const Token& identifier, const Object& value);
    std::string toString();
};