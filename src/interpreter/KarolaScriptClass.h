#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "KarolaScriptCallable.h"
#include "../util/Object.h"
//#include "../parser/Stmt.h"
//#include "../parser/Expr.h"

class Interpreter;
struct Token;

class KarolaScriptMetaClass;

class KarolaScriptClass : public KarolaScriptCallable, public std::enable_shared_from_this<KarolaScriptClass> {
public:
    std::string m_ClassName;
    std::optional<SharedCallablePtr> m_Superclass;
    std::unordered_map<std::string, Object> m_Methods;
    std::unordered_map<std::string, Object> m_StaticMethods;
    KarolaScriptMetaClass* metaClass;
public:
    KarolaScriptClass(const std::string& name_,
                      const std::optional<SharedCallablePtr> superclass_,
                      const std::unordered_map<std::string, Object>& methods_,
                      const std::unordered_map<std::string, Object>& staticMethods_
                      );

    Object call(Interpreter& interpreter, const std::vector<Object>& arguments) override;
    std::optional<Object> findMethod(const std::string& name);
    std::optional<Object> findStaticMethod(const std::string& name);
    Object getProperty(const Token& identifier);
    int arity() override;
    std::string toString() override;
    std::string name() override;
};

class KarolaScriptMetaClass : public KarolaScriptClass {
private:
    KarolaScriptMetaClass() : KarolaScriptClass("MetaClass", nullptr, {}, {}) {}

public:
    KarolaScriptMetaClass(KarolaScriptMetaClass const&) = delete;
    void operator=(KarolaScriptMetaClass const&) = delete;

    static KarolaScriptMetaClass& getInstance() {
        static KarolaScriptMetaClass instance;
        return instance;
    }

    static KarolaScriptClass* createClass(const std::string& clazzName, std::optional<SharedCallablePtr> superclass, std::unordered_map<std::string, Object> methods, std::unordered_map<std::string, Object> staticMethods) {
        return new KarolaScriptClass(clazzName, superclass, methods, staticMethods);
    }
};

class KarolaScriptInstance : public std::enable_shared_from_this<KarolaScriptInstance> {
private:
    std::shared_ptr<KarolaScriptClass> m_Klass;
    std::unordered_map<std::string, Object> m_Fields;
public:
    explicit KarolaScriptInstance(std::shared_ptr<KarolaScriptClass> klass_);
    Object getProperty(const Token& identifier);
    void setProperty(const Token& identifier, const Object& value);
    std::string toString();
};