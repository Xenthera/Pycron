//
// Created by Bobby on 9/28/2024.
//

#pragma once

#include <string>

enum TokenType{
    Keyword,
    Identifier,
    Literal,
    Operator,
    Punctuation,
    EndOfFile,
    Unknown
};

struct Token{
    TokenType type;
    std::string value;

    Token(TokenType t, const std::string& v)
            : type(t), value(v) {}
};

class PythonTokenizer {

};
