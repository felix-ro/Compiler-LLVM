#ifndef AST_H
#define AST_H

#include <string>

// Base Expression for all AST members
class ExprAST {
public:
    virtual ~ExprAST() = default;
};

// NumExprAST -> Expression class for numeric literals
class NumberExprAST : public ExprAST {
    double Val;

public: 
    NumberExprAST(double Num) : Val(Num) {}
};

// VariableExprAST -> Expression class for variable names such as 'a'
class VariableExprAST : public ExprAST {
    std::string Val;

public: 
    VariableExprAST(const std::string& Val) : Val(Val) {}
};

// A binary operation between two ExprAST (such as an addition)
class BinaryExprAST : public ExprAST {
    char Op; // The operation to perform
    std::unique_ptr<ExprAST> LHS, RHS; // Left and right hand side of operation

public: 
    BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS) 
        : Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

// CallExprAST -> represents a function call
class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

public:
    CallExprAST(const std::string& Callee,
                std::vector<std::unique_ptr<ExprAST>> Args) : Callee(Callee), Args(std::move(Args)) {}
};

// PrototypeExprAST -> The signature of a function definition
class PrototypeAST {
    std::string Name; 
    std::vector<std::string> Args;

public:
    PrototypeAST(const std::string& Name,
                std::vector<std::string> Args)
        : Name(Name), Args(std::move(Args)) {}
};

// FunctionExprAST -> The signature and function body
class FunctionAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;

public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                std::unique_ptr<ExprAST> Body) 
        : Proto(std::move(Proto)), Body(std::move(Body)) {}
};

#endif // AST_H