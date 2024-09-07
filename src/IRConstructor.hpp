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

#include "ast.hpp"

class IRConstructor {
    public:
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

    IRConstructor() {
        namedValues = std::map<std::string, llvm::Value*>(); 
        context = std::make_unique<llvm::LLVMContext>();
        module = std::make_unique<llvm::Module>("jitcompile", *context);
        builder = std::make_unique<llvm::IRBuilder<>>(*context);
        // module->setDataLayout()

        /* Analysis Managers */ 
        FPM = std::make_unique<llvm::FunctionPassManager>();
        LAM = std::make_unique<llvm::LoopAnalysisManager>(); 
        FAM = std::make_unique<llvm::FunctionAnalysisManager>();
        CGAM = std::make_unique<llvm::CGSCCAnalysisManager>();
        MAM = std::make_unique<llvm::ModuleAnalysisManager>();
        PIC = std::make_unique<llvm::PassInstrumentationCallbacks>();

        SI = std::make_unique<llvm::StandardInstrumentations>(*context, /* DebugLogging */ true);
        SI->registerCallbacks(*PIC, MAM.get());

        /* ----- Transformation Passes ------ */
        /* Peephole optimizations */ 
        FPM->addPass(llvm::InstCombinePass()); 
        /* Reassociate scalars (2 + 4 + x == x + 2 + 3) */
        FPM->addPass(llvm::ReassociatePass());
        /* Global Value Numbering (GVN) performs Common Subexpression Elimination (CSE) */
        FPM->addPass(llvm::GVNPass());
        /* Simplify the control flow graph (deleting unreachable blocks, etc). */
        FPM->addPass(llvm::SimplifyCFGPass());

        llvm::PassBuilder pb;
        pb.registerModuleAnalyses(*MAM);
        pb.registerFunctionAnalyses(*FAM);
        pb.crossRegisterProxies(*LAM, *FAM, *CGAM, *MAM);
    }

    llvm::Value* visit(NumberExprAST& numExpr);
    llvm::Value* visit(VariableExprAST& varExpr);
    llvm::Value* visit(BinaryExprAST& binExpr);
    llvm::Value* visit(CallExprAST& callExp);
    llvm::Function* visit(PrototypeAST& protoAST);
    llvm::Function* visit(FunctionAST& funcAST);
};
#endif // IRCONSTRUCTOR_H