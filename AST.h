#include <iostream>
#include <vector>
#include "DataTypes.h"
#include <string>

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
AST_NODE* parse_compare(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_language(std::vector<Token>& tokens, size_t& index);

AST_NODE* parse(std::vector<Token>& tokens, size_t& index) { //look for numbers and variable names
    if (tokens[index].type == INTEGER || tokens[index].type == IDENTIFIER || tokens[index].type == STRING) {
        AST_NODE* node = new AST_NODE{ tokens[index], nullptr, nullptr };
        index++;
        return node;
    }
    return nullptr;
}

AST_NODE* parse_parenthesis(std::vector<Token>& tokens, size_t& index) { //look for parentheses
    if (tokens[index].type == LEFT_PARENTHESIS) {
        index++;
        AST_NODE* expression = parse_compare(tokens, index);
        if (tokens[index].type != RIGHT_PARENTHESIS) {
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

AST_NODE* parse_compare(std::vector<Token>& tokens, size_t& index) { //look for comparisons
    AST_NODE* left = parse_lower(tokens, index);
    while (tokens[index].type == EQUAL || tokens[index].type == MORE || tokens[index].type == LESS || tokens[index].type == MORE_EQUAL || tokens[index].type == LESS_EQUAL) {
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
        if (tokens[index].type != INTEGER_IDENTIFIER && tokens[index].type != STRING_IDENTIFIER) {
            std::cerr << "Syntax Error: Variable type needed after 'create'" << std::endl;
            exit(1);
        }
        TokenType var_type;
        if (tokens[index].type == INTEGER_IDENTIFIER) var_type = INTEGER;
        if (tokens[index].type == STRING_IDENTIFIER) var_type = STRING;
        index++;
        Token variable_name = tokens[index++];
        if (var_type == INTEGER) {
            variables_integer[variable_name.value] = "0";
            variables_type[variable_name.value] = INTEGER;
        }
        if (var_type == STRING) {
            variables_string[variable_name.value] = "";
            variables_type[variable_name.value] = STRING;
        }
        return new AST_NODE{ variable_name, nullptr, nullptr };
    }
    if (tokens[index].type == OUTPUT) {
        index++;
        AST_NODE* expression = parse_compare(tokens, index);
        return new AST_NODE{ Token{OUTPUT, ""}, expression, nullptr };
    }
    if (tokens[index].type == INPUT) {
        index++;
        if (tokens[index].type != IDENTIFIER) {
            std::cerr << "Syntax Error: Expected variable name after 'in'" << std::endl;
            exit(1);
        }
        Token variable_name = tokens[index++];
        return new AST_NODE{ variable_name, nullptr, nullptr };
    }
    if (tokens[index].type == IDENTIFIER && index + 1 < tokens.size() && tokens[index + 1].type == ASSIGN) {
        Token var_token = tokens[index];
        index += 2;
        AST_NODE* expression = parse_compare(tokens, index);
        return new AST_NODE{ Token{ASSIGN, "="}, new AST_NODE{var_token, nullptr, nullptr}, expression };
    }
    return parse_compare(tokens, index);
}

Token evaluate(AST_NODE* node) {
    if (node->token.type == INTEGER) {
        return { INTEGER, node->token.value };
    }
    if (node->token.type == STRING) {
        return { STRING, node->token.value };
    }
    if (node->token.type == IDENTIFIER) {
        if (variables_type[node->token.value] == INTEGER) {
            return { INTEGER, variables_integer[node->token.value] };
        }
        if (variables_type[node->token.value] == STRING) {
            return { STRING, variables_integer[node->token.value] };
        }
        std::cerr << "Error: Undefined variable '" << node->token.value << "'" << std::endl;
        exit(1);
    }
    if (node->token.type == OUTPUT) {
        Token value = evaluate(node->left);
        std::cout << "> " << value.value << std::endl;
        return value;
    }
    if (node->token.type == INPUT) {
        Token value;
        std::cin >> value.value;
        if (variables_type[node->token.value] == INTEGER) {
            variables_integer[node->token.value] = INTEGER;
            value.type = INTEGER;
        }
        if (variables_type[node->token.value] == STRING) {
            variables_integer[node->token.value] = STRING;
            value.type = STRING;
        }
        return value;
    }
    Token left_val = evaluate(node->left);
    if (node->token.type == ASSIGN) {
        if (node->left->token.type != IDENTIFIER) {
            std::cerr << "Error: Left side of assignment must be a variable" << std::endl;
            exit(1);
        }
        std::string var_name = node->left->token.value;
        Token value = evaluate(node->right);
        if (variables_type[var_name] != value.type) {
            std::cerr << "Error: Assign wrong value type to the variable" << std::endl;
            exit(1);
        }
        if (value.type == INTEGER) {
            variables_integer[var_name] = value.value;
        }
        if (value.type == STRING) {
            variables_integer[var_name] = value.value;
        }
        return value;
    }
    Token right_val = evaluate(node->right);
    if (node->token.type == PLUS) {
        if (left_val.type == STRING) {
            return { STRING, left_val.value + right_val.value };
        }
        if (right_val.type == INTEGER) {
            return { INTEGER, std::to_string(stoi(left_val.value) + stoi(right_val.value)) };
        }
    }
    if (node->token.type == MINUS) {
        if (left_val.type == STRING) {
            std::cerr << "Error: You can't subtract strings from one another" << std::endl;
            exit(1);
        }
        return { INTEGER, std::to_string(stoi(left_val.value) - stoi(right_val.value)) };
    }
    if (node->token.type == MULTIPLY) {
        if (left_val.type == STRING) {
            std::cerr << "Error: You can't multiply strings together" << std::endl;
            exit(1);
        }
        return { INTEGER, std::to_string(stoi(left_val.value) * stoi(right_val.value)) };
    }
    if (node->token.type == DIVIDE) {
        if (left_val.type == STRING) {
            std::cerr << "Error: You can't divide strings by one another" << std::endl;
            exit(1);
        }
        if (right_val.value == "0") {
            std::cerr << "Error: Division by zero" << std::endl;
            exit(1);
        }
        return { INTEGER, std::to_string(stoi(left_val.value) / stoi(right_val.value)) };
    }
    if (node->token.type == MORE) {
        return { INTEGER, std::to_string(left_val.value > right_val.value) };
    }
    if (node->token.type == MORE_EQUAL) {
        return { INTEGER, std::to_string(left_val.value >= right_val.value) };
    }
    if (node->token.type == LESS) {
        return { INTEGER, std::to_string(left_val.value < right_val.value) };
    }
    if (node->token.type == LESS_EQUAL) {
        return { INTEGER, std::to_string(left_val.value <= right_val.value) };
    }
    if (node->token.type == EQUAL) {
        return { INTEGER, std::to_string(left_val.value == right_val.value) };
    }
    return { NONE, "" };
}

void FREE_AST(AST_NODE* node) { //delete the AST =)
    if (node) {
        FREE_AST(node->left);
        FREE_AST(node->right);
        delete node;
    }
}
