#include "ast.hpp"

#include <iostream>

llvm::LLVMContext& IRConstructor::getContext() const {
    return *Context;
}

llvm::IRBuilder<>& IRConstructor::getBuilder() const {
    return *Builder;
}

llvm::Module& IRConstructor::getModule() const {
    return *Module;
}

std::map<std::string, llvm::Value*>& IRConstructor::getNamedValues() {
    return NamedValues;
}

llvm::Value * LogErrorV(const std::string error) {
    std::cout << "Error: " << error << std::endl;
    return nullptr;
}

llvm::Value * ExprAST::codegen() {
    return nullptr;
}

llvm::Value * NumberExprAST::codegen() {
    return llvm::ConstantFP::get(*irConst->Context, llvm::APFloat(Val));
}

llvm::Value * VariableExprAST::codegen() {
    llvm::Value *V = irConst->NamedValues[Val];
    if (!V) {
        return LogErrorV("Unknown Variable Name"); 
    }
    return V;
}

llvm::Value * BinaryExprAST::codegen() {
    llvm::Value * l = LHS->codegen();
    llvm::Value * r = RHS->codegen();

    if (!l || !r) {
        return nullptr;
    }

    switch(Op) {
        case '+':
            return irConst->Builder->CreateFAdd(l, r, "addtmp");
        case '*':
            return irConst->Builder->CreateFMul(l, r, "multmp");
        case '-':
            return irConst->Builder->CreateFSub(l, r, "subtmp");
        case '<':
            l = irConst->Builder->CreateFCmpULT(l, r, "cmptmp");
            // our language currently does not have a bool type so we need to convert to double
            return irConst->Builder->CreateUIToFP(l, llvm::Type::getDoubleTy(*irConst->Context), "booltmp");
        default:
            return LogErrorV("Invalid Binary Operator");
    }
}

llvm::Value * CallExprAST::codegen() {
    llvm::Function * calleeF = irConst->Module->getFunction(Callee);

    if (!calleeF) {
        return LogErrorV("Unknown Function referenced");
    }

    if (calleeF->arg_size() != Args.size()) {
        return LogErrorV("Function and referenced function have an unequal number of arguments");
    }

    std::vector<llvm::Value *> argsV;
    argsV.reserve(Args.size());
    for (auto& arg : Args) {
        argsV.push_back(arg->codegen());

        if (!argsV.back()) {
            // make sure codegen succeeded
            return LogErrorV("Failed to codegen arguments");
        }
    }

    return irConst->Builder->CreateCall(calleeF, argsV, "calltmp");
}

llvm::Function * PrototypeAST::codegen() {
    // all arguments are double currently
    std::vector<llvm::Type *> doubles(Args.size(), llvm::Type::getDoubleTy(*irConst->Context));

    // get the function type, e.g., double(double, double)
    llvm::FunctionType *funcT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*irConst->Context),
                                                         doubles, false);

    llvm::Function *func = llvm::Function::Create(funcT, llvm::Function::ExternalLinkage,
                                                  Name, irConst->Module.get());
    
    // set the variable names
    size_t idx = 0;
    for (auto& arg : func->args()) {
        arg.setName(Args[idx++]);
    }

    return func;
}

std::string PrototypeAST::getName() const {
    return Name;
}

llvm::Function * FunctionAST::codegen() {
    llvm::Function * func = irConst->Module->getFunction(Proto->getName());

    if (!func) {
        func = Proto->codegen();
        if (!func) {
            return (llvm::Function*) LogErrorV("Failed to codegen function");
        }
    }

    if (!func->empty()) {
        return (llvm::Function*) LogErrorV("Cannot redefine function");
    }

    // Create a new basic block to start insertion into.
    llvm::BasicBlock *bb = llvm::BasicBlock::Create(*irConst->Context, "entry", func);
    irConst->Builder->SetInsertPoint(bb);

    // Record the function arguments in the NamedValues map.
    irConst->NamedValues.clear();
    for (auto& arg : func->args()) {
       irConst->NamedValues[std::string(arg.getName())] = &arg;
    }

    if (llvm::Value *retV = Body->codegen()) {
        irConst->Builder->CreateRet(retV);
        llvm::verifyFunction(*func);

        return func;
    } else {
        func->eraseFromParent();
        return nullptr;
    }
}