#include "IRConstructor.hpp"

#include "utils.hpp"

llvm::Value* NumberExprAST::codegen(IRConstructor& visitor) {
    return visitor.visit(*this);
}

llvm::Value* VariableExprAST::codegen(IRConstructor& visitor) {
    return visitor.visit(*this);
}

llvm::Value* BinaryExprAST::codegen(IRConstructor& visitor) {
    return visitor.visit(*this);
}

llvm::Value* CallExprAST::codegen(IRConstructor& visitor) {
    return visitor.visit(*this);
}

llvm::Function* PrototypeAST::codegen(IRConstructor& visitor) {
    return visitor.visit(*this);
}

llvm::Function* FunctionAST::codegen(IRConstructor& visitor) {
    return visitor.visit(*this);
}

llvm::Value* IRConstructor::visit(NumberExprAST& numExpr) {
    return llvm::ConstantFP::get(*this->context, llvm::APFloat(numExpr.getValue()));
}

llvm::Value* IRConstructor::visit(VariableExprAST& varExpr) {
    llvm::Value *V = namedValues[varExpr.getValue()];
    if (!V) {
        return LogErrorV("Unknown Variable Name"); 
    }
    return V;
}

llvm::Value* IRConstructor::visit(BinaryExprAST& binExpr) {
    llvm::Value * l = binExpr.getLHSRef().codegen(*this);
    llvm::Value * r = binExpr.getRHSRef().codegen(*this);

    if (!l || !r) {
        return nullptr;
    }

    switch(binExpr.getOp()) {
        case '+':
            return builder->CreateFAdd(l, r, "addtmp");
        case '*':
            return builder->CreateFMul(l, r, "multmp");
        case '-':
            return builder->CreateFSub(l, r, "subtmp");
        case '<':
            l = builder->CreateFCmpULT(l, r, "cmptmp");
            // our language currently does not have a bool type so we need to convert to double
            return builder->CreateUIToFP(l, llvm::Type::getDoubleTy(*context), "booltmp");
        default:
            return LogErrorV("Invalid Binary Operator");
    }
}

llvm::Value* IRConstructor::visit(CallExprAST& callExp) {
    llvm::Function * calleeF = module->getFunction(callExp.getCallee());

    if (!calleeF) {
        return LogErrorV("Unknown Function referenced");
    }

    size_t numArgs = callExp.getArgs().size();
    if (calleeF->arg_size() != numArgs) {
        return LogErrorV("Function and referenced function have an unequal number of arguments");
    }

    std::vector<llvm::Value *> argsV;
    argsV.reserve(numArgs);
    for (auto& arg : callExp.getArgs()) {
        argsV.push_back(arg->codegen(*this));

        if (!argsV.back()) {
            // make sure codegen succeeded
            return LogErrorV("Failed to codegen arguments");
        }
    }

    return builder->CreateCall(calleeF, argsV, "calltmp");
}


llvm::Function* IRConstructor::visit(PrototypeAST& protoAST) {
    // all arguments are double currently
    std::vector<llvm::Type *> doubles(protoAST.getArgs().size(), llvm::Type::getDoubleTy(*context));

    // get the function type, e.g., double(double, double)
    llvm::FunctionType *funcT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context),
                                                         doubles, false);

    llvm::Function *func = llvm::Function::Create(funcT, llvm::Function::ExternalLinkage,
                                                  protoAST.getName(), module.get());
    
    // set the variable names
    size_t idx = 0;
    for (auto& arg : func->args()) {
        arg.setName(protoAST.getArgs()[idx++]);
    }

    return func;
}

llvm::Function* IRConstructor::visit(FunctionAST& funcAST) {
    llvm::Function * func = module->getFunction(funcAST.getProto().getName());

    if (!func) {
        func = funcAST.getProto().codegen(*this);
        if (!func) {
            return (llvm::Function*) LogErrorV("Failed to codegen function");
        }
    }

    if (!func->empty()) {
        return (llvm::Function*) LogErrorV("Cannot redefine function");
    }

    // Create a new basic block to start insertion into.
    llvm::BasicBlock *bb = llvm::BasicBlock::Create(*context, "entry", func);
    builder->SetInsertPoint(bb);

    // Record the function arguments in the NamedValues map.
    namedValues.clear();
    for (auto& arg : func->args()) {
       namedValues[std::string(arg.getName())] = &arg;
    }

    if (llvm::Value *retV = funcAST.getBody().codegen(*this)) {
        builder->CreateRet(retV);
        llvm::verifyFunction(*func);

        return func;
    } else {
        func->eraseFromParent();
        return nullptr;
    }
}