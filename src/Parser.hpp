#ifndef PARSER_H
#define PARSER_H

#include "Lexer.hpp"
#include "AST.hpp"
#include "IRConstructor.hpp"

class Parser {
    Lexer lexer;
    Token curToken;
    std::unordered_map<char, int> binopPrecedence;
    std::shared_ptr<IRConstructor> irConst;

public:
    Parser(const std::string& input) : lexer(input), curToken(lexer.gettok()) {
        binopPrecedence.emplace('+', 10);
        binopPrecedence.emplace('-', 20);
        binopPrecedence.emplace('*', 30);

        irConst = std::make_shared<IRConstructor>();
    }

    int parse();

private: 
    void NextToken();
    void HandleDefinition();
    void HandleExtern();
    void HandleTopLevelExpression();
    std::unique_ptr<FunctionAST> ParseDefinition();
    std::unique_ptr<PrototypeAST> ParseExtern();
    std::unique_ptr<PrototypeAST> ParseProto();
    std::unique_ptr<ExprAST> ParseExpression();
    std::unique_ptr<ExprAST> ParsePrimary();
    std::unique_ptr<ExprAST> ParseNumberExpr();
    std::unique_ptr<ExprAST> ParseIdentifierExpr();
    std::unique_ptr<ExprAST> ParseParenExpr();
    std::unique_ptr<ExprAST> ParseBinOpRHS(int exprPrecedence, std::unique_ptr<ExprAST> lhs);

    int GetOperatorPrecedence();
};

#endif // PARSER_H