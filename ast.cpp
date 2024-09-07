#include "ast.hpp"
#include "utils.hpp"

#include <iostream>


double NumberExprAST::getValue() const {
    return value;
}

std::string VariableExprAST::getValue() const {
    return value;
}

ExprAST& BinaryExprAST::getLHSRef() const {
    return *LHS;
}

ExprAST& BinaryExprAST::getRHSRef() const {
    return *RHS;
}

char BinaryExprAST::getOp() const {
    return op;
}

std::string CallExprAST::getCallee() const {
    return callee;
}

std::vector<std::unique_ptr<ExprAST>>& CallExprAST::getArgs() {
    return args; 
}

std::string PrototypeAST::getName() const {
    return name;
}

std::vector<std::string>& PrototypeAST::getArgs() {
    return args;
}

PrototypeAST& FunctionAST::getProto() const {
    return *proto;
}

ExprAST& FunctionAST::getBody() const {
    return *body;
}