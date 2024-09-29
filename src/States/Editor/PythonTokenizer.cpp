//
// Created by Bobby on 9/28/2024.
//

#include <algorithm>
#include <iostream>
#include "PythonTokenizer.h"

PythonTokenizer::PythonTokenizer() {
    m_keywords = {"False", "None", "True", "and", "as", "assert", "async", "await", "break", "class", "continue", "def",
                  "del", "elif", "else", "except", "finally", "for", "from", "global", "if", "import", "in", "is", "lambda",
                  "nonlocal", "not", "or", "pass", "raise", "return", "try", "while", "with", "yield", "self"};

    m_builtins = {"abs", "all", "any", "ascii", "bin", "bool", "breakpoint", "bytearray", "bytes", "callable", "chr", "classmethod",
                  "compile", "complex", "delattr", "dict", "dir", "divmod", "enumerate", "eval", "exec", "filter", "float", "format",
                  "frozenset", "getattr", "globals", "hasattr", "hash", "help", "hex", "id", "input", "int", "isinstance", "issubclass",
                  "iter", "len", "list", "locals", "map", "max", "memoryview", "min", "next", "object", "oct", "open", "ord", "pow",
                  "print", "property", "range", "repr", "reversed", "round", "set", "setattr", "slice", "sorted", "staticmethod",
                  "str", "sum", "super", "tuple", "type", "vars", "zip", "__import__", "await", "__init__"};
}

std::vector<Token> PythonTokenizer::tokenizeLine(const std::string &line) {
    m_currentPos = 0;
    std::vector<Token> tokens;

    while(m_currentPos < line.length()){
        char currentChar = line[m_currentPos];

        if(std::isspace(currentChar))
        {
            std::string whitespace = readWhitespace(line);
            tokens.emplace_back(TokenType::WhiteSpace, whitespace);
        }
        else if(std::isalpha(currentChar) || currentChar == '_')
        {

            std::string identifier = readIdentifier(line);
            TokenType type;
            if(isKeyword(identifier)){
                type = TokenType::Keyword;
            }else if(isBuiltin(identifier)){
                type = TokenType::Builtin;
            }else{
                type = TokenType::Identifier;
            }
            tokens.emplace_back(type, identifier);
        }
        else if(currentChar == '"')
        {
            std::string identifier = readStringLiteral(line);
            tokens.emplace_back(TokenType::StringLiteral, identifier);
        }
        else if(std::isdigit(currentChar) || currentChar == '.')
        {
            std::string numericLiteral = readNumericLiteral(line);
            if(numericLiteral.empty())
            { // tested the . for any valid numbers, it failed so punctuation it is.
                std::string punct(1, currentChar);
                tokens.emplace_back(TokenType::Punctuation, punct);
                m_currentPos++;
            }
            tokens.emplace_back(TokenType::NumericalLiteral, numericLiteral);
        }
        else if(isPunctuation(currentChar))
        {
            std::string punct(1, currentChar);
            tokens.emplace_back(TokenType::Punctuation, punct);
            m_currentPos++;
        }
        else if(isOperator(currentChar))
        {
            std::string oper(1, currentChar);
            tokens.emplace_back(TokenType::Operator, oper);
            m_currentPos++;
        }
        else if(currentChar == '#')
        {
            std::string comment = readComment(line);
            tokens.emplace_back(TokenType::Comment, comment);
        }
        else
        {
            tokens.emplace_back(TokenType::Unknown, std::string(1, currentChar));
            m_currentPos++;
        }
    }

    return tokens;
}

std::string PythonTokenizer::readWhitespace(const std::string& line) {
    size_t start = m_currentPos;
    while (m_currentPos < line.length() && std::isspace(line[m_currentPos])) {
        m_currentPos++;
    }
    return line.substr(start, m_currentPos - start);
}

std::string PythonTokenizer::readIdentifier(const std::string &line) {
    size_t start = m_currentPos;
    while (m_currentPos < line.length() && (std::isalnum(line[m_currentPos]) || line[m_currentPos] == '_')) {
        m_currentPos++;
    }
    return line.substr(start, m_currentPos - start);
}

bool PythonTokenizer::isKeyword(const std::string &identifier) {
    return std::find(m_keywords.begin(), m_keywords.end(), identifier) != m_keywords.end();
}

bool PythonTokenizer::isBuiltin(const std::string &identifier) {
    return std::find(m_builtins.begin(), m_builtins.end(), identifier) != m_builtins.end();;
}

bool PythonTokenizer::isPunctuation(char c) {
    return c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == ',' || c == '.';
}

bool PythonTokenizer::isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '=';
}

std::string PythonTokenizer::readNumericLiteral(const std::string &line) {
    size_t start = m_currentPos;
    bool hasDigitsBeforeDecimal = false;
    bool hasDigitsAfterDecimal = false;

    // Read the integer part (if it exists)
    while (m_currentPos < line.length() && std::isdigit(line[m_currentPos])) {
        m_currentPos++;
        hasDigitsBeforeDecimal = true;
    }

    // Check for a decimal point
    if (m_currentPos < line.length() && line[m_currentPos] == '.') {
        size_t decimalStart = m_currentPos;
        m_currentPos++; // Move past the decimal point

        // Ensure there's at least one digit after the decimal point
        if (m_currentPos < line.length() && std::isdigit(line[m_currentPos])) {
            hasDigitsAfterDecimal = true; // Mark as valid if we see digits after the decimal

            // Continue reading the fractional part
            while (m_currentPos < line.length() && std::isdigit(line[m_currentPos])) {
                m_currentPos++;
            }
        } else {
            // If there are no digits before or after the decimal, treat the decimal as punctuation
            m_currentPos = decimalStart; // Revert to before the decimal point
        }
    }

    // Only return a literal if we've encountered at least one digit
    if (hasDigitsBeforeDecimal || hasDigitsAfterDecimal) {
        return line.substr(start, m_currentPos - start);
    } else {
        // Handle error case where no valid number was found
        return "";
    }

}

std::string PythonTokenizer::readStringLiteral(const std::string &line) {
    size_t start = m_currentPos;
    m_currentPos++;
    while (m_currentPos < line.length() && line[m_currentPos] != '"') {
        m_currentPos++;
        std::cout << m_currentPos;

    }

    if(line[m_currentPos] == '"') m_currentPos++;

    return line.substr(start, m_currentPos - start);
}

std::string PythonTokenizer::readComment(const std::string &line) {
    size_t start = m_currentPos;
    while (m_currentPos < line.length()) {
        m_currentPos++;
    }
    return line.substr(start, m_currentPos - start);
}
