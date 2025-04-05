#ifndef PARSER_H
#define PARSER_H
#include <iostream>
#include <vector>
#include "Converters.h"
#include "Values.h"
#include <string>
#include <unordered_set>
#include "Storage.h"

//we should always skip END type, our code cannot handle that

AST_NODE* parse(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_index(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_parenthesis(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_higher(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_lower(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_compare(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_language(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_list(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_function_arg(std::vector<Token>& tokens, size_t& index);

AST_NODE* parse(std::vector<Token>& tokens, size_t& index) { //look for numbers and variable names and functioncalls
    if (tokens[index].type == IDENTIFIER && index + 1 < tokens.size() && tokens[index + 1].type == LEFT_PARENTHESIS && function_body.count(tokens[index].name)) {
        return parse_function_arg(tokens, index);
    }
    if (tokens[index].type == INTEGER || tokens[index].type == IDENTIFIER || tokens[index].type == CHAR || tokens[index].type == LIST || tokens[index].type == STRING || tokens[index].type == BOOLEAN) {
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
    else if (tokens[index].type == CURLY_LEFT) {
        return parse_list(tokens, index);
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

AST_NODE* parse_list(std::vector<Token>& tokens, size_t& index) { //look for lists
    if (tokens[index].type != CURLY_LEFT) {
        return nullptr;
    }
    index++;
    std::vector<AST_NODE*> list;
    while (index < tokens.size() && tokens[index].type != CURLY_RIGHT) {
        if (tokens[index].type == COMMA) {
            index++;
            continue;
        }
        AST_NODE* cur_value = parse_compare(tokens, index);
        if (cur_value) {
            list.push_back(cur_value);
        }
        else {
            std::cerr << "Error: Invalid list element" << std::endl;
            exit(1);
        }
    }
    if (index < tokens.size() && tokens[index].type == CURLY_RIGHT) {
        index++;
    }
    else {
        std::cerr << "Syntax Error: Missing closing curly bracket for list" << std::endl;
        exit(1);
    }
    AST_NODE* list_node = new AST_NODE{ Token{LIST, 0, 0, {}, "list"}, nullptr, nullptr };
    AST_NODE* current = list_node;
    for (auto* it : list) {
        current->right = it;
        current = it;
    }
    return list_node;
}

AST_NODE* parse_function_arg(std::vector<Token>& tokens, size_t& index) {
    std::string func_name = tokens[index++].name;
    if(tokens[index].type != LEFT_PARENTHESIS){
        return nullptr;
    }
    index++;
    std::vector<AST_NODE*> arguments;
    while (index < tokens.size() && tokens[index].type != RIGHT_PARENTHESIS) {
        if (tokens[index].type == COMMA) {
            index++;
            continue;
        }
        AST_NODE* cur_val = parse_compare(tokens, index);
        if(cur_val) {
            arguments.push_back(cur_val);
        }
        else {
            std::cerr << "Error: Invalid argument" << std::endl;
            exit(1);
        }
    }
    if (index < tokens.size() && tokens[index].type == RIGHT_PARENTHESIS) {
        index++;
    }
    else {
        std::cerr << "Syntax Error: Missing closing parenthesis for function argument" << std::endl;
        exit(1);
    }
    AST_NODE* arg_node = new AST_NODE{Token{FUNCTION_CALL, 0, 0, {}, func_name}, nullptr, nullptr};
    arg_node->children = arguments;
    return arg_node;
}

AST_NODE* parse_language(std::vector<Token>& tokens, size_t& index) { //look for declaration, output, input, assign
    if (tokens[index].type == NEW_VAR) {
        index++;
        TokenType var_type;
        if (tokens[index].type == INTEGER_IDENTIFIER) var_type = INTEGER;
        else if (tokens[index].type == CHAR_IDENTIFIER) var_type = CHAR;
        else if (tokens[index].type == LIST_IDENTIFIER) var_type = LIST;
        else if (tokens[index].type == STRING_IDENTIFIER) var_type = STRING;
        else if (tokens[index].type == BOOLEAN_IDENTIFIER) var_type = BOOLEAN;
        else if (tokens[index].type == FUNCTION_IDENTIFIER) var_type = FUNCTION;
        else {
            std::cerr << "Syntax Error: Variable type needed after 'new'" << std::endl;
            exit(1);
        }
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
            return new AST_NODE{ variable_name, nullptr, nullptr };
        }
        else if (var_type == CHAR) {
            variables_char[variable_name.name] = ' ';
            variables_type[variable_name.name] = CHAR;
            return new AST_NODE{ variable_name, nullptr, nullptr };
        }
        else if (var_type == LIST) {
            variables_list[variable_name.name] = {};
            variables_type[variable_name.name] = LIST;
            return new AST_NODE{ variable_name, nullptr, nullptr };
        }
        else if (var_type == STRING) {
            variables_list[variable_name.name] = {};
            variables_type[variable_name.name] = STRING;
            return new AST_NODE{ variable_name, nullptr, nullptr };
        }
        else if (var_type == BOOLEAN) {
            variables_integer[variable_name.name] = 0;
            variables_type[variable_name.name] = BOOLEAN;
            return new AST_NODE{ variable_name, nullptr, nullptr };
        }
        else if (var_type == FUNCTION) {
            if (tokens[index].type != LEFT_PARENTHESIS) {
                std::cerr << "Syntax Error: Missing '(' for function argument";
                exit(1);
            }
            else index++;
            std::vector<std::string> args;
            while (index < tokens.size() && tokens[index].type != RIGHT_PARENTHESIS) {
                if (tokens[index].type == COMMA) {
                    index++;
                    continue;
                }
                else if (tokens[index].type == IDENTIFIER) {
                    args.push_back(tokens[index].name);
                    index++;
                }
                else {
                    std::cerr << "Error: Element in function argument during declaration must be of type 'IDENTIFIER'" << std::endl;
                    exit(1);
                }
            }
            if (index < tokens.size() && tokens[index].type == RIGHT_PARENTHESIS) {
                function_arguments[variable_name.name] = args;
                index++;
            }
            else {
                std::cerr << "Error: Missing closing parenthesis for function arguments" << std::endl;
                exit(1);
            }
            while (index < tokens.size() && tokens[index].type != CURLY_LEFT) {
                index++;
            }
            if (index == tokens.size()) {
                std::cerr << "Error: Missing body for function declaration" << std::endl;
                exit(1);
            }
            else index++;
            in_function_body++;
            cur_function_name = variable_name.name;
            return new AST_NODE{ variable_name, nullptr, nullptr };
        }
    }
    else if (tokens[index].type == RETURN) {
        index++;
        AST_NODE* expression = parse_compare(tokens, index);
        return new AST_NODE{ Token{RETURN}, expression, nullptr};
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
    else if (tokens[index].type == GETLINE) {
        index++;
        if (tokens[index].type != IDENTIFIER) {
            std::cerr << "Syntax Error: Expected variable name after 'in'" << std::endl;
            exit(1);
        }
        Token variable_name = tokens[index++];
        if (variables_type[variable_name.name] != STRING) {
            std::cerr << "Error: You cannot use getline for non-string values" << std::endl;
            exit(1);
        }
        return new AST_NODE{ Token{GETLINE, ' ', 0}, new AST_NODE{variable_name, nullptr, nullptr}, nullptr };
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
