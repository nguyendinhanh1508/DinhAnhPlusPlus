#ifndef VALUES_H
#define VALUES_H
#include <iostream>
#include <vector>
#include <cctype>
#include <unordered_map>
#include <unordered_set>

enum TokenType {
    INTEGER, CHAR, LIST, STRING, BOOLEAN, INDEX,
    INTEGER_IDENTIFIER, CHAR_IDENTIFIER, LIST_IDENTIFIER, STRING_IDENTIFIER, BOOLEAN_IDENTIFIER,
    PLUS, MINUS, MULTIPLY, DIVIDE,
    ASSIGN, IDENTIFIER,
    OUTPUT, INPUT, GETLINE, 
    NEW_VAR,
    LEFT_PARENTHESIS, RIGHT_PARENTHESIS,
    GET_VALUE, INDEX_END,
    MORE, MORE_EQUAL, LESS, LESS_EQUAL, EQUAL, NOT_EQUAL,
    END, NONE
};

struct list_element {
    int type;
    int integer;
    char character;
    std::vector<list_element> list;
};

std::unordered_map<std::string, int> variables_integer;
std::unordered_map<std::string, char> variables_char;
std::unordered_map<std::string, std::vector<list_element>> variables_list;
std::unordered_map<std::string, TokenType> variables_type;
std::unordered_set<std::string> already_declared;

bool func = false;
bool in_func = false;
bool in_parameter = false;

struct Token {
    TokenType type;
    char character;
    int integer;
    std::vector<list_element> list;
    std::string name;
};

std::unordered_map<std::string, std::vector<Token>> function_content;
std::unordered_map<std::string, std::vector<std::string>> function_parameter;
std::vector<std::string> cur_func_parameter;
std::vector<Token> cur_func_content;
std::string cur_func_name;

struct EvaluateValue {
    TokenType type;
    char character;
    int integer;
    std::vector<list_element> list;
    std::string name;
};

struct AST_NODE {
    Token token;
    AST_NODE* left;
    AST_NODE* right;
};

#endif
