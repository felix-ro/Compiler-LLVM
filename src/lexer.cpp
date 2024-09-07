#include "Lexer.hpp"

#include <cctype>
#include <iostream>

Lexer::Lexer(const std::string& input) {
    for (int i = input.size() - 1; i >= 0; i--) {
        st.push(input[i]);
    }
}

// Function to get the next character from the input string
char Lexer::getStrChar() {
    if (!st.empty()) {
        char c = st.top();
        st.pop();
        return c;
    }
    return EOF;
}

// Function to get the next token from the input string
Token Lexer::gettok() {
    std::string res;
    double val = 0.0;
    char last_char = getStrChar();

    while (isspace(last_char)) { // Skip whitespace
        last_char = getStrChar();
    }

    if (isalpha(last_char)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
        res.push_back(last_char);
        while (isalnum(st.top())) {
            res.push_back(getStrChar());
        }

        if (res == "def") {
            return Token(tok_def);
        } 
        if (res == "extern") {
            return Token(tok_extern);
        }
        if (res[0] == EOF) {
            return Token(tok_eof);
        }
        return Token(tok_identifier, res);
    }

    if (isdigit(last_char) || last_char == '.') { // number: [0-9.]+
        std::string num_str;
        num_str.push_back(last_char);
        while (!st.empty() && (isdigit(st.top()) || st.top() == '.')) {
            last_char = getStrChar();
            num_str.push_back(last_char);
        }
        val = strtod(num_str.c_str(), nullptr);
        return Token(tok_number, "", val);
    }

    if (last_char == EOF) {
        return Token(tok_eof);
    }

    // If none of the above, return an unknown token (default case)
    return Token(0, std::string(1, last_char));
}
