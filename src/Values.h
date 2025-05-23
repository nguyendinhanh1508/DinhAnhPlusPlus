#ifndef VALUES_H
#define VALUES_H
#include <iostream>
#include <vector>
#include <cctype>
#include <unordered_map>
#include <unordered_set>

enum TokenType {
    INTEGER, CHAR, LIST, STRING, BOOLEAN, FUNCTION, INDEX,
    IF, ELSE, FOR, WHILE,
    FUNCTION_CALL, RETURN, MUTABLE,
    INTEGER_IDENTIFIER, CHAR_IDENTIFIER,
    LIST_IDENTIFIER, STRING_IDENTIFIER, BOOLEAN_IDENTIFIER,
    FUNCTION_IDENTIFIER,
    PLUS, MINUS, MULTIPLY, DIVIDE, AND, OR, XOR, LEFT_SHIFT, RIGHT_SHIFT, MODULO,
    ASSIGN, IDENTIFIER,
    OUTPUT, INPUT, GETLINE,
    NEW_VAR,
    COMMA, SEMICOLON,
    LEFT_PARENTHESIS, RIGHT_PARENTHESIS,
    CURLY_LEFT, CURLY_RIGHT,
    GET_VALUE, INDEX_END,
    MORE, MORE_EQUAL, LESS, LESS_EQUAL, EQUAL, NOT_EQUAL, AND_BOOL, OR_BOOL, 
    END, NONE
};

struct list_element {
    int type;
    int integer;
    char character;
    std::vector<list_element> list;
};

struct Token {
    TokenType type;
    char character;
    int integer;
    std::vector<list_element> list;
    std::string name;
};

struct Pointer {
    int* integer = nullptr;
    char* character = nullptr;
    std::vector<list_element>* list = nullptr;
    list_element* list_element = nullptr;
};

struct EvaluateValue {
    TokenType type;
    char character;
    int integer;
    std::vector<list_element> list;
    std::string name;
    int index;
    bool isreturn = false;
    bool islist_element = false;
    Pointer pointer;
};

struct AST_NODE {
    Token token;
    AST_NODE* left;
    AST_NODE* right;
    std::vector<AST_NODE*> children;
    bool ismutable = false;
};

struct function_parameter {
    std::string name;
    TokenType type;
};
#endif
