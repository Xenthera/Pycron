//
// Created by Bobby on 9/28/2024.
//

#pragma once

#include <string>
#include <vector>

enum TokenType{
    Keyword,
    Builtin,
    Identifier,
    NumericalLiteral,
    StringLiteral,
    Operator,
    Punctuation,
    WhiteSpace,
    Comment,
    Unknown
};

struct Token{
    TokenType type;
    std::string value;

    Token(TokenType t, const std::string& v)
            : type(t), value(v) {}
};

class PythonTokenizer {
public:
    PythonTokenizer();

    std::vector<Token> tokenizeLine(const std::string& line);

private:
    size_t m_currentPos;

    std::vector<std::string> m_keywords;
    std::vector<std::string> m_builtins;

    bool isKeyword(const std::string& identifier);
    bool isBuiltin(const std::string& identifier);
    bool isPunctuation(char c);
    bool isOperator(char c);
    std::string readWhitespace(const std::string& line);
    std::string readIdentifier(const std::string& line);
    std::string readNumericLiteral(const std::string& line);
    std::string readStringLiteral(const std::string& line);
    std::string readComment(const std::string& line);

};
