#ifndef AST_H
#define AST_H

#include <string>
#include <map>
#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"


class IRConstructor {
    public:
    std::unique_ptr<llvm::LLVMContext> Context;
    std::unique_ptr<llvm::IRBuilder<>> Builder;
    std::unique_ptr<llvm::Module> Module;
    std::map<std::string, llvm::Value*> NamedValues;

    IRConstructor() {
        Context = std::make_unique<llvm::LLVMContext>();
        Module = std::make_unique<llvm::Module>("jitcompile", *Context);
        Builder = std::make_unique<llvm::IRBuilder<>>(*Context);
        NamedValues = std::map<std::string, llvm::Value*>(); 
    }

    llvm::LLVMContext& getContext() const;
    llvm::IRBuilder<>& getBuilder() const;
    llvm::Module& getModule() const;
    std::map<std::string, llvm::Value*>& getNamedValues();
};

// Base Expression for all AST members
class ExprAST {
public:
    virtual ~ExprAST() = default;
    virtual llvm::Value *codegen() = 0;
};

// NumExprAST -> Expression class for numeric literals
class NumberExprAST : public ExprAST {
    double Val;
    std::shared_ptr<IRConstructor> irConst;

public: 
    NumberExprAST(double Num, std::shared_ptr<IRConstructor> irConst) : Val(Num), irConst(irConst) {}
    llvm::Value *codegen() override;
};

// VariableExprAST -> Expression class for variable names such as 'a'
class VariableExprAST : public ExprAST {
    std::string Val;
    std::shared_ptr<IRConstructor> irConst;

public: 
    VariableExprAST(const std::string& Val, std::shared_ptr<IRConstructor> irConst)
                : Val(Val), irConst(irConst) {}
    llvm::Value *codegen() override;
};

// A binary operation between two ExprAST (such as an addition)
class BinaryExprAST : public ExprAST {
    char Op; // The operation to perform
    std::unique_ptr<ExprAST> LHS, RHS; // Left and right hand side of operation
    std::shared_ptr<IRConstructor> irConst;

public: 
    BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS,
                  std::shared_ptr<IRConstructor> irConst) 
        : Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)), irConst(irConst) {}
    llvm::Value *codegen() override;
};

// CallExprAST -> represents a function call
class CallExprAST : public ExprAST {
    std::shared_ptr<IRConstructor> irConst;
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

public:
    CallExprAST(const std::string& Callee, std::vector<std::unique_ptr<ExprAST>> Args,
                std::shared_ptr<IRConstructor> irConst) 
            : Callee(Callee), Args(std::move(Args)), irConst(irConst) {}
    llvm::Value *codegen() override;
};

// PrototypeExprAST -> The signature of a function definition
class PrototypeAST {
    std::shared_ptr<IRConstructor> irConst;
    std::string Name; 
    std::vector<std::string> Args;

public:
    PrototypeAST(const std::string& Name, std::vector<std::string> Args,
                 std::shared_ptr<IRConstructor> irConst)
        : Name(Name), Args(std::move(Args)), irConst(irConst) {}
    
    llvm::Function *codegen();
    std::string getName() const;
};

// FunctionExprAST -> The signature and function body
class FunctionAST {
    std::shared_ptr<IRConstructor> irConst;
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;

public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body,
                std::shared_ptr<IRConstructor> irConst) 
        : Proto(std::move(Proto)), Body(std::move(Body)), irConst(irConst) {}
    llvm::Function *codegen();
};

#endif // AST_H