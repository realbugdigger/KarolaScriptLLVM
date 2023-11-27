#include "KarolaScriptAnonFunction.h"

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
#include "RuntimeError.h"

KarolaScriptAnonFunction::KarolaScriptAnonFunction(const AnonFunction* declaration_,
                                           std::shared_ptr<Environment> closure_
                                           )
        : KarolaScriptCallable(CallableType::ANON_FUNCTION), m_Declaration(declaration_), m_Closure(std::move(closure_)) {}

Object KarolaScriptAnonFunction::call(Interpreter& interpreter, const std::vector<Object>& arguments) {
    std::shared_ptr<Environment> environment = std::make_shared<Environment>(m_Closure);

    if (!arguments.empty()) {
        for (int i = 0; i < m_Declaration->m_Params.size(); i++) { // m_Declaration->m_Params.size() == arguments.size() => HAS TO BE!!!
//            if (!arguments[i].isNull() && (arguments[i].type == ObjType::OBJTYPE_CALLABLE && arguments[i].getCallable()->m_Type == CallableType::ANON_FUNCTION)/*ObjType::OBJTYPE_ANONFUNCTION*/) {
//            std::vector<UniqueStmtPtr> ptr = arguments[i].getAnonFunction()->m_Body;
            if (!arguments[i].isNull() && arguments[i].isAnonFunction()) {
                AnonFunction* functStmt = new AnonFunction(/*m_Declaration->m_Params[i],*/ arguments[i].getAnonFunction()->m_Params, arguments[i].getAnonFunction()->m_Body);
                std::shared_ptr<KarolaScriptAnonFunction> ksFunct = std::make_unique<KarolaScriptAnonFunction>(functStmt, environment);
                Object anonFunctObject(ksFunct);
                environment->define(m_Declaration->m_Params[i], anonFunctObject);
            } else {
                environment->define(m_Declaration->m_Params[i], arguments[i]);
            }


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
//                environment->define(m_Declaration->m_Params[i], arguments[i]); // environment->define(m_Declaration->m_Params[i].lexeme, arguments[i]); ????
//            }
        }
    }

    try {
        interpreter.executeBlock(m_Declaration->m_Body, environment);
    } catch (ReturnException& returnValue) {
        /*NOTE: We're using exceptions as control flow here because it is the cleanest way to implement return given
        how the book implements the interpreter. This exception was thrown in the visitReturnStmt method of the interpreter*/

        return returnValue.m_Value;
    }

    return Object::Null();
}

int KarolaScriptAnonFunction::arity() {
    return m_Declaration->m_Params.size();
}