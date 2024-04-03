#include <iostream>
#include "CodeGenVisitor.h"

// TODO
// Make some "LLVM IR CodeGenException"
#include "../interpreter/RuntimeError.h"

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

llvm::Value *CodeGenVisitor::visitGetExpr(Get &expr) {
    llvm::Value *val = environment->lookup(expr.m_Name.lexeme);
    if (val == nullptr) {
//        throw new CodeGenException(std::string("Var not found: " + var.varName));
    }
    return val;
}

llvm::Value *CodeGenVisitor::visitAssignExpr(Assign &expr) {
    if (expr.m_Value == nullptr) {
//        throw new IRCodeGenException(
//                std::string("Assigning a null expr to " + expr.identifier->varName));
    }
    llvm::Value *assignedVal = expr.m_Value->codegen(*this);
    llvm::Value *id = expr.identifier->codegen(*this);
    if (id == nullptr) {
//        throw new CodeGenException(std::string("Trying to assign to a null id: " +
//                                                 expr.identifier->varName));
    }
    auto distance = localsDistances.find(&expr);
    if (distance != localsDistances.end()) {
        environment->assignAt(distance->second, expr.m_Name.lexeme, assignedVal);
    } else {
        globals->assign(expr.m_Name.lexeme, assignedVal);
    }
    builder->CreateStore(assignedVal, id);
    return assignedVal;
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
    compileFunction(&stmt);
}

void CodeGenVisitor::visitBreakStmt(Break &stmt) {
//    To generate equivalent LLVM IR for break statement, you need to utilize IRBuilder
//    to create a branch instruction to the corresponding block where the loop ends.

//    However, remember the information about the loop end block should be stored
//    somewhere in your compiler as it is not managed by LLVM itself.

//    ```
//    llvm::IRBuilder<> builder(the_context);
//    llvm::BasicBlock* BreakBlock; // initialized to point at the block we need to break to.
//
//    // assuming BreakBlock initialized properly...
//    builder.CreateBr(BreakBlock);
//    ```

//    Where BreakBlock is BasicBlock that represents the end of the loop in which the break statement occurs.
//
//    Note: In a real scenario, BreakBlock should be maintained in a stack data structure as a part of your
//    loop handling code in your compiler so that nested break statements can function as expected
}

void CodeGenVisitor::visitLetStmt(Let &stmt) {
    if (expr.boundExpr == nullptr) {
        throw new IRCodegenException(
                std::string("Let - binding a null expr to " + expr.varName));
    }
    llvm::Value *boundVal = expr.boundExpr->codegen(*this);

    // put allocainst in entry block of parent function, to be optimised by
    // mem2reg
    llvm::Function *parentFunction = builder->GetInsertBlock()->getParent();
    llvm::IRBuilder<> TmpBuilder(&(parentFunction->getEntryBlock()),
                                 parentFunction->getEntryBlock().begin());
    llvm::AllocaInst *var = TmpBuilder.CreateAlloca(boundVal->getType(), nullptr,
                                                    llvm::Twine(expr.varName));
    varEnv[expr.varName] = var;
    builder->CreateStore(boundVal, var);
    return boundVal;









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
    stmt->accept(this);
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
        return environment->getAt(localsDistances[variableExpr], identifier.lexeme);
    }
    return globals->lookup(identifier.lexeme);
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

    auto varAlloc = builder->CreateAlloca(type_, nullptr, name);

    // add to the environment
    environment->define(name, varAlloc);

    return varAlloc;
}

llvm::Value *CodeGenVisitor::compileFunction(const Function* functExpr) {
    auto params = functExpr->m_Params;
    auto body = hasReturnType(functExpr) ? fnExp.list[5] : fnExp.list[3];

    // Save current
    auto prevFn = fn;
    auto prevBlock = builder->GetInsertBlock();

    // Override fn to compile body
    llvm::Function* newFn = getOrCreateFunction(functExpr->m_Name.lexeme, extractFunctionType(*functExpr));
    fn = newFn;

    // Set parameter names
    int idx = 0;

    // Function environment for params
    // Enter a new environment.
    EnvironmentGuard environment_guard{*this, std::move(environment)};;

    for (auto& arg : fn->args()) {
        Token param = params[idx++];
        auto argName = extractVarName(param);

        arg.setName(argName);

        // Allocate a local variable per argument to make arguments mutable.
        llvm::Value* argBinding = allocVar(argName, arg.getType());
        builder->CreateStore(&arg, argBinding);
    }

    builder->CreateRet(gen(body));

    // Restore previous fn after compiling.
    builder->SetInsertPoint(prevBlock);
    fn = prevFn;

    return newFn;
}

llvm::Function *CodeGenVisitor::getOrCreateFunction(const std::string &fnName, llvm::FunctionType *fnType) {
    // Function prototype might already be defined
    llvm::Function* llvmFn = module->getFunction(fnName);

    // If not, allocate the function
    if (llvmFn == nullptr) {
        llvmFn = createFunctionPrototype(fnName, fnType);
    }

    createFunctionBlock(llvmFn);
    return llvmFn;
}

llvm::Function *CodeGenVisitor::createFunctionPrototype(const std::string &fnName, llvm::FunctionType *fnType) {
    // Function prototype might already be defined:
    llvm::Function* llvmFn = llvm::Function::Create(fnType, llvm::Function::ExternalLinkage, fnName, *module);

    // Validate the generated code, checking for consistency.
    llvm::verifyFunction(*llvmFn);
    return llvmFn;
}

llvm::FunctionType *CodeGenVisitor::extractFunctionType(const Function &stmt) {
    auto params = stmt.m_Params;

    // Return type
    auto returnType = hasReturnType(fnExp)
                      ? getTypeFromString(fnExp.list[4].string)
                      : builder->getInt32Ty();

    // Parameter types
    std::vector<llvm::Type*> paramTypes{};

    for (auto& param : params) {
        auto paramTy = extractVarType(param);
        paramTypes.push_back(paramTy);
    }

    return llvm::FunctionType::get(returnType, paramTypes, /* varargs */ false);
}

void CodeGenVisitor::createFunctionBlock(llvm::Function *llvmFn) {
    auto entry = createBasicBlock("entry", llvmFn);
    builder->SetInsertPoint(entry);
}

llvm::BasicBlock *CodeGenVisitor::createBasicBlock(const std::string& name, llvm::Function *llvmFn) {
    return llvm::BasicBlock::Create(*ctx, name, llvmFn);
}