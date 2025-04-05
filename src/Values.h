#ifndef VALUES_H
#define VALUES_H
#include <iostream>
#include <vector>
#include <cctype>
#include <unordered_map>
#include <unordered_set>

int in_list = 0;
int in_function_body = 0;

enum TokenType {
    INTEGER, CHAR, LIST, STRING, BOOLEAN, FUNCTION, INDEX,
    FUNCTION_ARG,
    INTEGER_IDENTIFIER, CHAR_IDENTIFIER,
    LIST_IDENTIFIER, STRING_IDENTIFIER, BOOLEAN_IDENTIFIER,
    FUNCTION_IDENTIFIER,
    PLUS, MINUS, MULTIPLY, DIVIDE,
    ASSIGN, IDENTIFIER,
    OUTPUT, INPUT, GETLINE,
    NEW_VAR,
    COMMA,
    LEFT_PARENTHESIS, RIGHT_PARENTHESIS,
    CURLY_LEFT, CURLY_RIGHT,
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
std::unordered_map<std::string, std::vector<std::string>> function_arguments;

struct Token {
    TokenType type;
    char character;
    int integer;
    std::vector<list_element> list;
    std::string name;
};

std::vector<Token> tokens;
std::unordered_map<std::string, std::vector<Token>> function_body;
std::vector<Token> cur_function_body;
std::string cur_function_name;

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
