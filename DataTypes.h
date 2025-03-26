#ifndef DATATYPES_H
#define DATATYPES_H
#include <iostream>
#include <vector>
#include <cctype>
#include <unordered_map>

enum TokenType {
    INTEGER, STRING, 
    INTEGER_IDENTIFIER, STRING_IDENTIFIER, 
    PLUS, MINUS, MULTIPLY, DIVIDE, 
    ASSIGN, IDENTIFIER, 
    OUTPUT, INPUT, 
    NEW_VAR,
    LEFT_PARENTHESIS, RIGHT_PARENTHESIS,
    MORE, MORE_EQUAL, LESS, LESS_EQUAL, EQUAL,
    END, NONE
};

std::unordered_map<std::string, std::string> variables_integer;
std::unordered_map<std::string, std::string> variables_string;
std::unordered_map<std::string, TokenType> variables_type;

struct Token {
    TokenType type;
    std::string value;
};

struct AST_NODE {
    Token token;
    AST_NODE* left;
    AST_NODE* right;
};
#endif
