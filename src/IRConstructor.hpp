#ifndef IRCONSTRUCTOR_H
#define IRCONSTRUCTOR_H

#include <string>
#include <map>
#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"

#include "ast.hpp"

class IRConstructor {
    public:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
    std::map<std::string, llvm::Value*> namedValues;

    IRConstructor() {
        context = std::make_unique<llvm::LLVMContext>();
        module = std::make_unique<llvm::Module>("jitcompile", *context);
        builder = std::make_unique<llvm::IRBuilder<>>(*context);
        namedValues = std::map<std::string, llvm::Value*>(); 
    }

    llvm::Value* visit(NumberExprAST& numExpr);
    llvm::Value* visit(VariableExprAST& varExpr);
    llvm::Value* visit(BinaryExprAST& binExpr);
    llvm::Value* visit(CallExprAST& callExp);
    llvm::Function* visit(PrototypeAST& protoAST);
    llvm::Function* visit(FunctionAST& funcAST);
};
#endif // IRCONSTRUCTOR_H