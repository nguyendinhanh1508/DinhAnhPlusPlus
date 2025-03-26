#ifndef DATATYPES_H
#define DATATYPES_H
#include <iostream>
#include <vector>
#include <cctype>
#include <unordered_map>

std::unordered_map<std::string, int> variables_integer;

enum TokenType {
    NUMBER, 
    PLUS, MINUS, MULTIPLY, DIVIDE, 
    ASSIGN, IDENTIFIER, 
    OUTPUT, INPUT, 
    NEW_VAR,
    LEFT_PARENTHESIS, RIGHT_PARENTHESIS,
    MORE, MORE_EQUAL, LESS, LESS_EQUAL, EQUAL,
    END
};

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
