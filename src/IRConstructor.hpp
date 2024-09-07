#ifndef IRCONSTRUCTOR_H
#define IRCONSTRUCTOR_H

#include <string>
#include <map>

// Core llvm types 
#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"

// Analysis Managers 
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"

// Optimization Passes 
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"

#include "AST.hpp"

class IRConstructor {
private:
    std::map<std::string, llvm::Value*> namedValues;
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;

    // Optimization
    std::unique_ptr<llvm::FunctionPassManager> FPM;
    std::unique_ptr<llvm::LoopAnalysisManager> LAM; 
    std::unique_ptr<llvm::FunctionAnalysisManager> FAM;
    std::unique_ptr<llvm::CGSCCAnalysisManager> CGAM;
    std::unique_ptr<llvm::ModuleAnalysisManager> MAM;
    std::unique_ptr<llvm::PassInstrumentationCallbacks> PIC;
    std::unique_ptr<llvm::StandardInstrumentations> SI;

public:
    IRConstructor();

    llvm::Value* visit(NumberExprAST& numExpr);
    llvm::Value* visit(VariableExprAST& varExpr);
    llvm::Value* visit(BinaryExprAST& binExpr);
    llvm::Value* visit(CallExprAST& callExp);
    llvm::Function* visit(PrototypeAST& protoAST);
    llvm::Function* visit(FunctionAST& funcAST);
};
#endif // IRCONSTRUCTOR_H