#ifndef DATATYPES_H
#define DATATYPES_H
#include <iostream>
#include <vector>
#include <cctype>
#include <unordered_map>
#include <unordered_set>

enum TokenType {
    INTEGER, STRING, LIST, CHAR,
    INTEGER_IDENTIFIER, STRING_IDENTIFIER, LIST_IDENTIFIER, CHAR_IDENTIFIER,
    PLUS, MINUS, MULTIPLY, DIVIDE, 
    ASSIGN, IDENTIFIER, 
    OUTPUT, INPUT, 
    NEW_VAR,
    LEFT_PARENTHESIS, RIGHT_PARENTHESIS,
    MORE, MORE_EQUAL, LESS, LESS_EQUAL, EQUAL,
    END, NONE
};

std::unordered_map<std::string, std::string> variables_integer;
std::unordered_map<std::string, std::string> variables_char;
std::unordered_map<std::string, std::vector<std::vector<std::string>>> variables_list_string;
std::unordered_map<std::string, std::vector<std::string>> variables_string;
std::unordered_map<std::string, std::vector<std::string>> variables_list_integer;
std::unordered_map<std::string, TokenType> variables_type;
std::unordered_set<std::string> already_declared;

struct Token {
    TokenType type;
    std::string value;
};

struct AST_NODE {
    Token token;
    AST_NODE* left;
    AST_NODE* right;
};

AST_NODE* parse(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_parenthesis(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_higher(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_lower(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_compare(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_language(std::vector<Token>& tokens, size_t& index);

#endif
