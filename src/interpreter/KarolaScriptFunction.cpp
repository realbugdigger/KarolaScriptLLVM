#include "KarolaScriptFunction.h"

#include <cassert>
#include <memory>
#include <sstream>
#include <utility>

#include "../parser/Expr.h"
#include "Interpreter.h"
#include "../parser/Stmt.h"
#include "../lexer/Token.h"
//#include "typedefs.h"
#include "KarolaScriptClass.h"

KarolaScriptFunction::KarolaScriptFunction(const Function* declaration_,
                                           std::shared_ptr<Environment> closure_,
                                           bool isInitializer_
                                                )
                    : KarolaScriptCallable(CallableType::FUNCTION), m_Declaration(declaration_), m_Closure(std::move(closure_)), m_IsInitializer_(isInitializer_) {}

Object KarolaScriptFunction::call(Interpreter& interpreter, const std::vector<Object>& arguments) {
    std::shared_ptr<Environment> environment = std::make_shared<Environment>(m_Closure);

    if (!arguments.empty()) {
        for (int i = 0; i < m_Declaration->m_Params.size(); i++) { // m_Declaration->m_Params.size() == arguments.size() => HAS TO BE!!!
//            if (arguments[i] && arguments[i]->type() == typeid(AnonFunction)) {
//                try {
//                    std::shared_ptr<Function> stmt = std::make_shared<Function>(m_Declaration->m_Params[i], std::any_cast<AnonFunction>(arguments[i]).m_Params, std::any_cast<AnonFunction>(arguments[i]).m_Body);
//                    std::shared_ptr<KarolaScriptFunction> function = std::make_shared<KarolaScriptFunction>(stmt, environment, false);
//                    environment->define(m_Declaration->m_Params[i].lexeme, function);
//                }
//                catch(const std::bad_any_cast& e) {
//                    std::cout << "Bad cast: " << e.what() << std::endl; // This should definitely never ever happen.
//                }
//            } else {
                environment->define(m_Declaration->m_Params[i], arguments[i]); // environment->define(m_Declaration->m_Params[i].lexeme, arguments[i]); ????
//            }
        }
    }

    try {
        interpreter.executeBlock(m_Declaration->m_Body, environment);
    } catch (ReturnException& returnValue) {
        /*NOTE: We're using exceptions as control flow here because it is the cleanest way to implement return given
        how the book implements the interpreter. This exception was thrown in the visitReturnStmt method of the interpreter*/

        //Constructor should always implicitly return "this".
        if (m_IsInitializer_) {
            return m_Closure->getAt(0, "this");
        }
        return returnValue.m_Value;
    }

    if (m_IsInitializer_) {
        //Constructor should always implicitly return "this". This line covers the case where the constructor has no return stmt
        //but we still need to return "this".
        return m_Closure->getAt(0, "this");
    }

    return Object::Null();
}

int KarolaScriptFunction::arity() {
    return m_Declaration->m_Params.size();
}

KarolaScriptFunction* KarolaScriptFunction::bind(SharedInstancePtr instance) {
    std::shared_ptr<Environment> environment = std::make_shared<Environment>(m_Closure);
    Object instanceObj(std::move(instance));
    environment->define("this", instanceObj);
    return new KarolaScriptFunction(m_Declaration, environment, m_IsInitializer_);
}

std::string KarolaScriptFunction::toString() {
    return "<fn " + name() + ">";
}

std::string KarolaScriptFunction::name() {
    return m_Declaration->m_Name.lexeme;
}