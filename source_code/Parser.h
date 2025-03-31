#ifndef PARSER_H
#define PARSER_H
#include <iostream>
#include <vector>
#include "Converters.h"
#include "Values.h"
#include <string>
#include <unordered_set>

AST_NODE* parse(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_index(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_parenthesis(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_higher(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_lower(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_compare(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_language(std::vector<Token>& tokens, size_t& index);

AST_NODE* parse(std::vector<Token>& tokens, size_t& index) { //look for numbers and variable names
    if (tokens[index].type == INTEGER || tokens[index].type == IDENTIFIER || tokens[index].type == CHAR || tokens[index].type == LIST || tokens[index].type == STRING) {
        AST_NODE* node = new AST_NODE{ tokens[index], nullptr, nullptr };
        index++;
        return node;
    }
    return nullptr;
}

AST_NODE* parse_index(std::vector<Token>& tokens, size_t& index) { //look for indices for list access
    AST_NODE* left = parse(tokens, index);
    while (index < tokens.size() && tokens[index].type == GET_VALUE) {
        index++;
        AST_NODE* expression = parse_compare(tokens, index);
        if (tokens[index].type != INDEX_END) {
            std::cerr << "Syntax Error: Expected ']' after index" << std::endl;
            exit(1);
        }
        index++;
        AST_NODE* node = new AST_NODE{ Token{GET_VALUE, 0, 0, {}, "[]"}, left, expression };
        left = node;
    }
    return left;
}

AST_NODE* parse_parenthesis(std::vector<Token>& tokens, size_t& index) { //look for parentheses
    if (tokens[index].type == LEFT_PARENTHESIS) {
        index++;
        AST_NODE* expression = parse_lower(tokens, index);
        if (tokens[index].type != RIGHT_PARENTHESIS) {
            std::cerr << "Syntax Error: Expected ')' after expression" << std::endl;
            exit(1);
        }
        index++;
        return expression;
    }
    return parse_index(tokens, index);
}

AST_NODE* parse_higher(std::vector<Token>& tokens, size_t& index) { //look for '*' and '/'
    AST_NODE* left = parse_parenthesis(tokens, index);
    while (tokens[index].type == MULTIPLY || tokens[index].type == DIVIDE) {
        Token operation = tokens[index++];
        AST_NODE* right = parse_parenthesis(tokens, index);
        AST_NODE* node = new AST_NODE{ operation, left, right };
        left = node;
    }
    return left;
}

AST_NODE* parse_lower(std::vector<Token>& tokens, size_t& index) { //look for '+' and '-'
    AST_NODE* left = parse_higher(tokens, index);
    while (tokens[index].type == PLUS || tokens[index].type == MINUS) {
        Token operation = tokens[index++];
        AST_NODE* right = parse_higher(tokens, index);
        AST_NODE* node = new AST_NODE{ operation, left, right };
        left = node;
    }
    return left;
}

AST_NODE* parse_compare(std::vector<Token>& tokens, size_t& index) { //look for comparisons
    AST_NODE* left = parse_lower(tokens, index);
    while (tokens[index].type == EQUAL || tokens[index].type == MORE || tokens[index].type == LESS || tokens[index].type == MORE_EQUAL || tokens[index].type == LESS_EQUAL || tokens[index].type == NOT_EQUAL) {
        Token operation = tokens[index++];
        AST_NODE* right = parse_lower(tokens, index);
        AST_NODE* node = new AST_NODE{ operation, left, right };
        left = node;
    }
    return left;
}

AST_NODE* parse_language(std::vector<Token>& tokens, size_t& index) { //look for declaration, output, input, assign
    if (tokens[index].type == NEW_VAR) {
        index++;
        if (tokens[index].type != INTEGER_IDENTIFIER && tokens[index].type != CHAR_IDENTIFIER && tokens[index].type != LIST_IDENTIFIER && tokens[index].type != STRING_IDENTIFIER) {
            std::cerr << "Syntax Error: Variable type needed after 'new'" << std::endl;
            exit(1);
        }
        TokenType var_type;
        if (tokens[index].type == INTEGER_IDENTIFIER) var_type = INTEGER;
        else if (tokens[index].type == CHAR_IDENTIFIER) var_type = CHAR;
        else if (tokens[index].type == LIST_IDENTIFIER) var_type = LIST;
        else if (tokens[index].type == STRING_IDENTIFIER) var_type = STRING;
        index++;
        Token variable_name = tokens[index++];
        if (already_declared.count(variable_name.name)) {
            std::cerr << "Error: Previously declared variable \'" << variable_name.name << '\'' << std::endl;
            exit(1);
        }
        already_declared.insert(variable_name.name);
        if (var_type == INTEGER) {
            variables_integer[variable_name.name] = 0;
            variables_type[variable_name.name] = INTEGER;
        }
        else if (var_type == CHAR) {
            variables_char[variable_name.name] = ' ';
            variables_type[variable_name.name] = CHAR;
        }
        else if (var_type == LIST) {
            variables_list[variable_name.name] = {};
            variables_type[variable_name.name] = LIST;
        }
        else if (var_type == STRING) {
            variables_list[variable_name.name] = {};
            variables_type[variable_name.name] = STRING;
        }
        return new AST_NODE{ variable_name, nullptr, nullptr };
    }
    else if (tokens[index].type == OUTPUT) {
        index++;
        AST_NODE* expression = parse_compare(tokens, index);
        return new AST_NODE{ Token{OUTPUT,' ', 0}, expression, nullptr };
    }
    else if (tokens[index].type == INPUT) {
        index++;
        if (tokens[index].type != IDENTIFIER) {
            std::cerr << "Syntax Error: Expected variable name after 'in'" << std::endl;
            exit(1);
        }
        Token variable_name = tokens[index++];
        return new AST_NODE{ Token{INPUT, ' ', 0}, new AST_NODE{variable_name, nullptr, nullptr}, nullptr };
    }
    else if (tokens[index].type == IDENTIFIER && index + 1 < tokens.size() && tokens[index + 1].type == ASSIGN) {
        Token var_token = tokens[index];
        index += 2;
        AST_NODE* expression = parse_compare(tokens, index);
        return new AST_NODE{ Token{ASSIGN, ' ', 0, {}, "="}, new AST_NODE{var_token, nullptr, nullptr}, expression };
    }
    return parse_compare(tokens, index);
}

void FREE_AST(AST_NODE* node) { //delete the AST =)
    if (node) {
        FREE_AST(node->left);
        FREE_AST(node->right);
        delete node;
    }
}
#endif
