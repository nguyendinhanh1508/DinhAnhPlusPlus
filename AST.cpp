#include <iostream>
#include <vector>
#include "DataTypes.h"

/*
the order is:
look for operations: input, output, declaration, or assign
look for parentheses
look for '*' and '/'
look for '+' and '-'
look for variables or numbers
*/

AST_NODE* parse(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_parenthesis(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_higher(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_lower(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_language(std::vector<Token>& tokens, size_t& index);

AST_NODE* parse(std::vector<Token>& tokens, size_t& index) { //look for numbers and variable names
    if (tokens[index].type == NUMBER || tokens[index].type == IDENTIFIER) {
        AST_NODE* node = new AST_NODE{ tokens[index], nullptr, nullptr };
        index++;
        return node;
    }
    return nullptr;
}

AST_NODE* parse_parenthesis(std::vector<Token>& tokens, size_t& index){ //look for parentheses
    if(tokens[index].type == LEFT_PARENTHESIS){
        index++;
        AST_NODE* expression = parse_lower(tokens, index);
        if(tokens[index].type != RIGHT_PARENTHESIS){
            std::cerr << "Syntax Error: Expected ')' after expression" << std::endl;
            exit(1);
        }
        index++;
        return expression;
    }
    return parse(tokens, index);
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

AST_NODE* parse_language(std::vector<Token>& tokens, size_t& index) { //look for declaration, output, input, assign
    if (tokens[index].type == NEW_VAR) {
        index++;
        if (tokens[index].type != IDENTIFIER) {
            std::cerr << "Syntax Error: Expected variable after 'create'" << std::endl;
            exit(1);
        }
        Token variable_name = tokens[index++];
        variables[variable_name.value] = 0;
        return new AST_NODE{variable_name, nullptr, nullptr};
    }
    if (tokens[index].type == OUTPUT) {
        index++;
        AST_NODE* expression = parse_lower(tokens, index);
        return new AST_NODE{Token{OUTPUT, ""}, expression, nullptr};
    }
    if (tokens[index].type == INPUT) {
        index++;
        if (tokens[index].type != IDENTIFIER) {
            std::cerr << "Syntax Error: Expected variable name after 'in'" << std::endl;
            exit(1);
        }
        Token variable_name = tokens[index++];
        return new AST_NODE{variable_name, nullptr, nullptr};
    }
    if (tokens[index].type == IDENTIFIER && index + 1 < tokens.size() && tokens[index+1].type == ASSIGN) {
        Token var_token = tokens[index];
        index += 2;
        AST_NODE* expr = parse_lower(tokens, index);
        return new AST_NODE{Token{ASSIGN, "="}, new AST_NODE{var_token, nullptr, nullptr}, expr};
    }
    return parse_lower(tokens, index);
}

int evaluate(AST_NODE* node) {
    if (node->token.type == NUMBER) {
        return std::stoi(node->token.value);
    }
    if (node->token.type == IDENTIFIER) {
        if (variables.count(node->token.value)) {
            return variables[node->token.value];
        }
        std::cerr << "Error: Undefined variable '" << node->token.value << "'" << std::endl;
        exit(1);
    }
    if (node->token.type == OUTPUT) {
        int value = evaluate(node->left);
        std::cout << value << std::endl;
        return value;
    }
    if (node->token.type == INPUT) {
        int value;
        std::cin >> value;
        variables[node->token.value] = value;
        return value;
    }
    int left_val = evaluate(node->left);
    if (node->token.type == ASSIGN) {
        if (node->left->token.type != IDENTIFIER) {
            std::cerr << "Error: Left side of assignment must be a variable" << std::endl;
            exit(1);
        }
        std::string var_name = node->left->token.value;
        int value = evaluate(node->right);
        variables[var_name] = value;
        return value;
    }
    int right_val = evaluate(node->right);
    if (node->token.type == PLUS) {
        return left_val + right_val;
    }
    if (node->token.type == MINUS) {
        return left_val - right_val;
    }
    if (node->token.type == MULTIPLY) {
        return left_val * right_val;
    }
    if (node->token.type == DIVIDE) {
        if (right_val == 0) {
            std::cerr << "Error: Division by zero" << std::endl;
            exit(1);
        }
        return left_val / right_val;
    }
    return 0;
}

void FREE_AST(AST_NODE* node) { //delete the AST =)
    if (node) {
        FREE_AST(node->left);
        FREE_AST(node->right);
        delete node;
    }
}