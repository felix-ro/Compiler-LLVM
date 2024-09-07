#ifndef AST_H
#define AST_H

#include <string>
#include <map>
#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"

class IRConstructor; // forward declaration

// Base Expression for all AST members
class ExprAST {
public:
    virtual ~ExprAST() = default;
    virtual llvm::Value* codegen(IRConstructor& visitor) = 0;
};

// NumExprAST -> Expression class for numeric literals
class NumberExprAST : public ExprAST {
    double value;
    
public: 
    NumberExprAST(double value) : value(value) {}
    llvm::Value* codegen(IRConstructor& visitor) override;
    double getValue() const;
};

// VariableExprAST -> Expression class for variable names such as 'a'
class VariableExprAST : public ExprAST {
    std::string value;

public: 
    VariableExprAST(const std::string& value) : value(value) {}
    
    llvm::Value* codegen(IRConstructor& visitor) override;
    std::string getValue() const; 
};

// A binary operation between two ExprAST (such as an addition)
class BinaryExprAST : public ExprAST {
    char op; // The operation to perform
    std::unique_ptr<ExprAST> LHS, RHS; // Left and right hand side of operation

public: 
    BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS) 
        : op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
    
    llvm::Value* codegen(IRConstructor& visitor) override;
    ExprAST& getLHSRef() const;
    ExprAST& getRHSRef() const;
    char getOp() const;
};

// CallExprAST -> represents a function call
class CallExprAST : public ExprAST {
    std::string callee;
    std::vector<std::unique_ptr<ExprAST>> args;

public:
    CallExprAST(const std::string& callee, std::vector<std::unique_ptr<ExprAST>> args) 
            : callee(callee), args(std::move(args)) {}
    
    llvm::Value* codegen(IRConstructor& visitor) override;
    std::string getCallee() const;
    std::vector<std::unique_ptr<ExprAST>>& getArgs();
};

// PrototypeExprAST -> The signature of a function definition
class PrototypeAST {
    std::string name; 
    std::vector<std::string> args;

public:
    PrototypeAST(const std::string& name, std::vector<std::string> args)
        : name(name), args(std::move(args)) {}
    
    llvm::Function* codegen(IRConstructor& visitor);
    
    std::string getName() const;
    std::vector<std::string>& getArgs();
};

// FunctionExprAST -> The signature and function body
class FunctionAST {
    std::unique_ptr<PrototypeAST> proto;
    std::unique_ptr<ExprAST> body;

public:
    FunctionAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<ExprAST> body) 
        : proto(std::move(proto)), body(std::move(body)) {}
    
    llvm::Function* codegen(IRConstructor& visitor);

    PrototypeAST& getProto() const; 
    ExprAST& getBody() const;
};

#endif // AST_H