#include "../include/Parser.hpp"

#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"


#include <iostream>

std::unique_ptr<PrototypeAST> LogErrorP(const std::string s) {
    std::cout << "Error: " << s << std::endl;
    return nullptr;
} 

std::unique_ptr<ExprAST> LogError(const std::string s) {
    std::cout << "Error: " << s << std::endl;
    return nullptr;
} 

int Parser::GetOperatorPrecedence() {
    char c = curToken.identifier[0];
    if (isascii(c) && binopPrecedence.find(c) != binopPrecedence.end()) {
        return binopPrecedence[c];
    }
    return -1;
}

std::unique_ptr<ExprAST> Parser::ParseBinOpRHS(int exprPrecedence, std::unique_ptr<ExprAST> lhs) {
    while(true) {
        int tokPrecedence = GetOperatorPrecedence();

        if (tokPrecedence < exprPrecedence) {
            return lhs;
        }

        char binOp = curToken.identifier[0];
        NextToken(); // drop binOp

        auto rhs = ParsePrimary();
        if (!rhs) {
            return nullptr;
        }

        // If BinOp binds less tightly with RHS than the operator after RHS, let
        // the pending operator take RHS as its LHS.
        int nextPrecedence = GetOperatorPrecedence();
        if (tokPrecedence < nextPrecedence) {
            rhs = ParseBinOpRHS(exprPrecedence + 1, std::move(rhs));
            if (!rhs)
                return nullptr;
        }

        // Merge LHS/RHS.
        lhs = std::make_unique<BinaryExprAST>(binOp, std::move(lhs), std::move(rhs));
    }
}

std::unique_ptr<ExprAST> Parser::ParseParenExpr() {
    NextToken(); // drop '(' and read next token
    auto exp = ParseExpression();

    if (!exp) {
        return nullptr;
    }

    if (curToken.identifier != ")") {
        LogError("Expected closing ')'");
    }
    NextToken(); // drop ')' and read next token;
    return exp;
}

std::unique_ptr<ExprAST> Parser::ParseIdentifierExpr() {
    const std::string id_name = curToken.identifier;
    NextToken(); // drop identifier token and read next

    if (curToken.identifier != "(") {
        // not a function call but rather a variable
        return std::make_unique<VariableExprAST>(id_name);
    }

    // parse Function Call
    NextToken(); // drop '(' and read next token
    std::vector<std::unique_ptr<ExprAST>> args;
    while (curToken.identifier != ")") {
        auto arg = ParseExpression();
        if (!arg) {
            return nullptr;
        }
        args.push_back(std::move(arg));

        if (curToken.identifier != ")" && curToken.identifier != ",") {
            return LogError("Expected ')' or ',' in argument list");
        }
        NextToken();
    }
    return std::make_unique<CallExprAST>(id_name, std::move(args)); 
}

std::unique_ptr<ExprAST> Parser::ParseNumberExpr() {
    double num = curToken.number_value;
    NextToken();
    return std::make_unique<NumberExprAST>(num);
}

std::unique_ptr<ExprAST> Parser::ParsePrimary() {
    switch(curToken.type) {
        case tok_number:
            return ParseNumberExpr();
        case tok_identifier:
            return ParseIdentifierExpr();
        default:
            // ToDo: add tokens for open and close
            if (curToken.identifier == "(") {
                return ParseParenExpr();
            } else {
                return LogError("Unrecognized token when parsing primary expression");
            }
    }
}

std::unique_ptr<ExprAST> Parser::ParseExpression() {
    auto lhs = ParsePrimary();
    if (lhs == nullptr) {
        return nullptr;
    }
    return ParseBinOpRHS(0, std::move(lhs));
}


std::unique_ptr<PrototypeAST> Parser::ParseProto() {
    if (curToken.type != tok_identifier) {
        return LogErrorP("Expected function name in prototype");
    }
    std::string functionName = curToken.identifier;
    NextToken();

    if (curToken.type != tok_unknown || curToken.identifier != "(" ) {
        return LogErrorP("Expected '(' to start prototype");
    }
    NextToken();

    std::vector<std::string> argNames;
    while(curToken.type == tok_identifier) {
        argNames.push_back(curToken.identifier);
        NextToken();
    }

    if (curToken.type != tok_unknown && curToken.identifier != ")") {
        return LogErrorP("Expected ')' to end prototype");
    }
    NextToken(); // drop closing ')' and get next token
    return std::make_unique<PrototypeAST>(functionName, std::move(argNames));
}

std::unique_ptr<FunctionAST> Parser::ParseDefinition() {
    NextToken(); // drop 'def' and get next token
    auto proto = ParseProto();
    if (!proto) {
        return nullptr;
    }

    if (auto exp = ParseExpression()) {
        return std::make_unique<FunctionAST>(std::move(proto), std::move(exp));
    }
    return nullptr;
}

std::unique_ptr<PrototypeAST> Parser::ParseExtern() {
    NextToken(); // drop extern
    return ParseProto();
}

void Parser::HandleDefinition() {
    if (auto fnAST = ParseDefinition()) {
        if (auto *fnIR = fnAST->codegen(*irConst)) {
            std::cout << "Read function definition" << std::endl;
            fnIR->print(llvm::errs());
            std::cout << std::endl;
        }
        std::cout << "Parsed Definiton" << std::endl;
    } else {
        std::cout << "Failed to parse definition" << std::endl;
        NextToken(); // skip for error recovery
    }
}

void Parser::HandleExtern() {
    if (auto protoAST = ParseExtern()) {
        if (auto *extIR = protoAST->codegen(*irConst)){
            std::cout << "Parsed Extern" << std::endl;
            extIR->print(llvm::errs()); 
            std::cout << std::endl;
        }
    } else {
        std::cout << "Failed to parse extern" << std::endl;
        NextToken(); 
    }
}

void Parser::HandleTopLevelExpression() {
    if (auto fnAST = ParseExpression()) {
        if (auto *fnIR = fnAST->codegen(*irConst)) {
            std::cout << "Parsed top level expression" << std::endl;
            fnIR->print(llvm::errs());
            std::cout << std::endl;
        }
    } else {
        std::cout << "Failed to parse top level expression" << std::endl;
        NextToken();
    }
}

void Parser::NextToken() {
    curToken = lexer.gettok();
}

/* 
We need to split AST and IR construction

- add pretty printing
- make everything modular
*/
int Parser::parse() {
    bool run = true; 

    /// top ::= definition | external | expression | ';'
    while (run) {
        switch (curToken.type) {
        case tok_eof:
            run = false; 
            break;
        case tok_def:
            HandleDefinition();
            break;
        case tok_extern:
            HandleExtern();
            break;
        default:
            HandleTopLevelExpression();
            run = false; 
            break;
        }
    }

    std::cout << "IR Generated" << std::endl;
    return 0;
}

std::shared_ptr<IRConstructor> Parser::GetIRConstructor() {
    return irConst;
}