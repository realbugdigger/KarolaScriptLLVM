#include "KarolaScriptAnonFunction.h"

#include <cassert>
#include <memory>
#include <sstream>
#include <utility>

#include "../parser/Expr.h"
#include "Interpreter.h"
#include "../parser/Stmt.h"
#include "../lexer/Token.h"
#include "KarolaScriptClass.h"
#include "RuntimeError.h"

KarolaScriptAnonFunction::KarolaScriptAnonFunction(const AnonFunction* declaration_,
                                           std::shared_ptr<Environment> closure_
                                           )
        : KarolaScriptCallable(CallableType::ANON_FUNCTION), m_Declaration(declaration_), m_Closure(std::move(closure_)) {}

Object KarolaScriptAnonFunction::call(Interpreter& interpreter, const std::vector<Object>& arguments) {
    std::shared_ptr<Environment> environment = std::make_shared<Environment>(m_Closure);

    if (!arguments.empty()) {
        for (int i = 0; i < m_Declaration->m_Params.size(); i++) { // m_Declaration->m_Params.size() == arguments.size() => HAS TO BE!!!
            environment->define(m_Declaration->m_Params[i], arguments[i]);
        }
    }

    try {
        interpreter.executeBlock(m_Declaration->m_Body, environment);
    } catch (ReturnException& returnValue) {
        /* NOTE: This exception was thrown in the visitReturnStmt method of the interpreter */
        return returnValue.m_Value;
    }

    return Object::Null();
}

int KarolaScriptAnonFunction::arity() {
    return m_Declaration->m_Params.size();
}