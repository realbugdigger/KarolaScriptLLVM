#include <iostream>
#include "CodeGenVisitor.h"

#include "../interpreter/RuntimeError.h"
#include "../interpreter/KarolaScriptFunction.h"

CodeGenVisitor::CodeGenVisitor(Interpreter& interpreter) : m_Interpreter(interpreter) {
    moduleInit();
    setupExternFunctions();
}

void CodeGenVisitor::generate(std::vector<UniqueStmtPtr> &statements) {
    for (auto& statement : statements) {
        // Compile to LLVM IR
        compile(statement.get());
    }

    // Print generated code.
    module->print(llvm::outs(), nullptr);

    std::cout << "\n";

    // Save module IR to file
    saveModuleToFile("./out.ll");
}

llvm::Value* CodeGenVisitor::visitVariableExpr(Variable &expr) {
    return lookupVariable(expr.m_VariableName, &expr);
}

llvm::Value *CodeGenVisitor::visitCallExpr(Call& expr) {
//    llvm::Value* callable = gen(expr.);
    Object callee = m_Interpreter.evaluate(expr.m_Callee.get());

    std::vector<llvm::Value*> args{};
    for (const UniqueExprPtr &arg : expr.m_Arguments) {
        Object argObject = m_Interpreter.evaluate(arg.get());

        if (argObject.isAnonFunction()) {
            KarolaScriptFunction* ksFunction = dynamic_cast<KarolaScriptFunction *>(callee.getCallable().get());
            environment->define(ksFunction->m_Declaration->m_Name.lexeme, argObject);
        }

        args.push_back(gen(argObject));
    }

//    auto fn = (llvm::Function*) callee;

    return builder->CreateCall(fn, args);
}

llvm::Value* CodeGenVisitor::visitBinaryExpr(Binary &expr) {
    Object leftObject = m_Interpreter.evaluate(expr.m_Left.get());
    Object rightObject = m_Interpreter.evaluate(expr.m_Left.get());

    llvm::Value* left = gen(leftObject);
    llvm::Value* right = gen(rightObject);

    switch (expr.m_Operator.type) {
        case TOKEN_MINUS:
            m_Interpreter.checkNumberOperands(expr.m_Operator, leftObject, rightObject);
            return builder->CreateSub(left, right);
        case TOKEN_SLASH:
            m_Interpreter.checkNumberOperands(expr.m_Operator, leftObject, rightObject);

            // Throw error if right operand is 0.
            if (rightObject.getNumber() == 0) {
                throw RuntimeError(expr.m_Operator, "Division by 0.");
            }
            return builder->CreateSDiv(left, right);

        case TOKEN_STAR:
            m_Interpreter.checkNumberOperands(expr.m_Operator, leftObject, rightObject);
            return builder->CreateMul(left, right);

        case TOKEN_GREATER:
            m_Interpreter.checkNumberOperands(expr.m_Operator, leftObject, rightObject);
            return builder->CreateICmpUGT(left, right);

        case TOKEN_GREATER_EQUAL:
            m_Interpreter.checkNumberOperands(expr.m_Operator, leftObject, rightObject);
            return builder->CreateICmpUGE(left, right);

        case TOKEN_LESS:
            m_Interpreter.checkNumberOperands(expr.m_Operator, leftObject, rightObject);
            return builder->CreateICmpULT(left, right);

        case TOKEN_LESS_EQUAL:
            m_Interpreter.checkNumberOperands(expr.m_Operator, leftObject, rightObject);
            return builder->CreateICmpULE(left, right);

        case TOKEN_EQUAL_EQUAL:
            return builder->CreateICmpEQ(left, right);

        case TOKEN_BANG_EQUAL:
            return builder->CreateICmpNE(left, right);

        case TOKEN_PLUS:
            if (leftObject.isString() && rightObject.isString()) {
                return builder->CreateGlobalString(leftObject.getString() + rightObject.getString());
            }
            else if (leftObject.isNumber() && rightObject.isNumber()) {
                return builder->CreateAdd(left, right);
            }
            else if (leftObject.isNumber() && rightObject.isString()) {
                // Remove trailing zeroes.
                std::string num_as_string = std::to_string(leftObject.getNumber());
                num_as_string.erase(num_as_string.find_last_not_of('0') + 1, std::string::npos);
                num_as_string.erase(num_as_string.find_last_not_of('.') + 1, std::string::npos);
                return builder->CreateGlobalString(num_as_string + rightObject.getString());
            }
            else if (leftObject.isString() && rightObject.isNumber()) {
                // Remove trailing zeroes.
                std::string num_as_string = std::to_string(rightObject.getNumber());
                num_as_string.erase(num_as_string.find_last_not_of('0') + 1, std::string::npos);
                num_as_string.erase(num_as_string.find_last_not_of('.') + 1, std::string::npos);
                return builder->CreateGlobalString(leftObject.getString() + num_as_string);
            }

            throw RuntimeError(expr.m_Operator, "Operands must be of type string or number.");

        default:
            return {};
    }
}

void CodeGenVisitor::visitFunctionStmt(Function &stmt) {
    createFunction(stmt.m_Name.lexeme, llvm::FunctionType::get(/* return type*/ builder->getInt32Ty(),true));
}

void CodeGenVisitor::visitLetStmt(Let &stmt) {
    // maybe check if already contains key with stmt.m_Name.lexeme, if yes throw RuntimeError

    Object value;
    // If the variable has an initializer, evaluate the initializer.
    if (stmt.m_Initializer.has_value()) {
        value = m_Interpreter.evaluate(stmt.m_Initializer->get());
    }

    // Define the variable in the current environment with the given identifier and value
    environment->define(stmt.m_Name.lexeme, value);
}

void CodeGenVisitor::visitWhileStmt(While &stmt) {
    // condition
    auto condBlock = createBasicBlock("cond", fn);
    builder->CreateBr(condBlock);

    // Body, while-end blocks
    auto bodyBlock = createBasicBlock("body");
    auto loopEndBlock = createBasicBlock("loopend");

    // Compile <cond>
    builder->SetInsertPoint(condBlock);
    auto cond = gen(exp.list[1]);

    // Condition branch
    builder->CreateCondBr(cond, bodyBlock, loopEndBlock);

    // Body
    fn->getBasicBlockList().push_back(bodyBlock);
    builder->SetInsertPoint(bodyBlock);
    generate(stmt.m_Body);
    builder->CreateBr(condBlock);

    fn->getBasicBlockList().push_back(loopEndBlock);
    builder->SetInsertPoint(loopEndBlock);

    builder->getInt32(0);
}

void CodeGenVisitor::compile(Stmt *stmt) {
    stmt->accept(*this);
}

void CodeGenVisitor::moduleInit() {
    // Open a new context and module.
    ctx = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("KarolaScript-LLVM", *ctx);

    // Create a new builder for the module.
    builder = std::make_unique<llvm::IRBuilder<>>(*ctx);
}

void CodeGenVisitor::setupExternFunctions() {
    // i8* is substitute for char*, void*, etc
    auto bytePtrTy = builder->getInt8Ty()->getPointerTo();

    // int printf(const char* format, ...);
    module->getOrInsertFunction("printf", llvm::FunctionType::get(/* return type*/ builder->getInt32Ty(),
                                                                /* format arg */ bytePtrTy,
                                                                /* vararg  */ true
                                                                ));

    // TODO
    // at one point `malloc` or some `GC_malloc` would need to be loaded also for heap allocations
}

// Saves IR to file
void CodeGenVisitor::saveModuleToFile(const std::string &fileName) {
    std::error_code errorCode;
    llvm::raw_fd_ostream outLL(fileName, errorCode);
    module->print(outLL, nullptr);
}

llvm::Value *CodeGenVisitor::gen(Object object) {
    if (object.isNumber())
        return builder->getInt64(object.getNumber());
    if (object.isString())
        return builder->CreateGlobalString(object.getString());
    if (object.isBoolean())
        return builder->getInt1(object.getBoolean());
}

llvm::Value* CodeGenVisitor::lookupVariable(const Token &identifier, const Expr *variableExpr) {
    if (localsDistances.find(variableExpr) != localsDistances.end()){
        Object o = environment->getAt(localsDistances[variableExpr], identifier.lexeme);
        if (!o.isNull()) {
            // create local var
            builder->SetInsertPoint(&fn->getEntryBlock());

            auto varAlloc = builder->CreateAlloca(type_, 0, name.c_str());

            // add to the environment:
            env->define(name, varAlloc);

            return varAlloc;
        }
    }

    Object o = globals->lookup(identifier);
    if (!o.isNull()) {
        // create global var
        module->getOrInsertGlobal(identifier.lexeme, init->getType());
        auto variable = module->getNamedGlobal(identifier.lexeme);
        variable->setAlignment(llvm::MaybeAlign(4));
        variable->setConstant(false);
        variable->setInitializer(init);
        return variable;
    }
}

size_t CodeGenVisitor::getTypeSize(llvm::Type *type_) {
    return module->getDataLayout().getTypeAllocSize(type_);
}

std::string CodeGenVisitor::extractVarName(Token token) {
    return token.lexeme;
}

llvm::Type *CodeGenVisitor::extractVarType() {

}

llvm::Value *CodeGenVisitor::allocVar(const std::string &name, llvm::Type *type_) {
    builder->SetInsertPoint(&fn->getEntryBlock());

    auto varAlloc = builder->CreateAlloca(type_, 0, name.c_str());

    // add to the environment
    env->define(name, varAlloc);

    return varAlloc;
}

llvm::Value *CodeGenVisitor::compileFunction(const Function* functExpr) {
    auto params = functExpr->m_Params;
    auto body = hasReturnType(functExpr) ? fnExp.list[5] : fnExp.list[3];

    // Save current
    auto prevFn = fn;
    auto prevBlock = builder->GetInsertBlock();

    // Override fn to compile body
    auto newFn = createFunction(fnName, extractFunctionType(fnExp), env);
    fn = newFn;

    // Set parameter names
    auto idx = 0;

    // Function environment for params:
    auto fnEnv = std::make_shared<Environment>(
            std::map<std::string, llvm::Value*>{}, env
    );

    for (auto& arg : fn->args()) {
        Token param = params[idx++];
        auto argName = extractVarName(param);

        arg.setName(argName);

        // Allocate a local variable per argument to make arguments mutable.
        llvm::Value* argBinding = allocVar(argName, arg.getType(), fnEnv);
        builder->CreateStore(&arg, argBinding);
    }

    builder->CreateRet(gen(body, fnEnv));

    // Restore previous fn after compiling.
    builder->SetInsertPoint(prevBlock);
    fn = prevFn;

    return newFn;
}

llvm::Function *CodeGenVisitor::createFunction(const std::string &fnName, llvm::FunctionType *fnType) {
    // Function prototype might already be defined:
    auto fn = module->getFunction(fnName);

    // If not, allocate the function:
    if (fn == nullptr) {
        fn = createFunctionPrototype(fnName, fnType);
    }

    createFunctionBlock(fn);
    return fn;
}

llvm::Function *CodeGenVisitor::createFunctionPrototype(const std::string &fnName, llvm::FunctionType *fnType) {
    // Function prototype might already be defined:
    auto fn = llvm::Function::Create(fnType, llvm::Function::ExternalLinkage, fnName, *module);

    // Validate the generated code, checking for consistency.
    llvm::verifyFunction(*fn);
    return fn;
}

void CodeGenVisitor::createFunctionBlock(llvm::Function *fn) {
    auto entry = createBasicBlock("entry", fn);
    builder->SetInsertPoint(entry);
}

llvm::BasicBlock *CodeGenVisitor::createBasicBlock(const std::string& name, llvm::Function *fn) {
    return llvm::BasicBlock::Create(*ctx, name, fn);
}