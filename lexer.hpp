#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <stack>

// Define enum for different tokens
enum Tokens {
    tok_unknown = 0,

    tok_eof = -1,

    // commands
    tok_def = -2, 
    tok_extern = -3,

    // primary
    tok_identifier = -4,
    tok_number = -5
};

// Struct to hold token information
struct Token {
    int type;
    std::string identifier;
    double number_value;

    Token(int t, const std::string& id = "", double num_val = 0.0)
        : type(t), identifier(id), number_value(num_val) {}
};

class Lexer {
std::stack<char> st;

public:
    Lexer(const std::string& input);

    Token gettok();

private: 
    char getStrChar();
};

#endif // LEXER_H