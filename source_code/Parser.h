#include <iostream>
#include <vector>
#include "Converters.h"
#include "DataTypes.h"
#include <string>
#include <unordered_set>

/*
the order is:
look for operations: input, output, declaration, or assign
look for parentheses
look for '*' and '/'
look for '+' and '-'
look for variables or numbers
*/

AST_NODE* parse(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_index(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_parenthesis(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_higher(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_lower(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_compare(std::vector<Token>& tokens, size_t& index);
AST_NODE* parse_language(std::vector<Token>& tokens, size_t& index);

AST_NODE* parse(std::vector<Token>& tokens, size_t& index) { //look for numbers and variable names
    if (tokens[index].type == INTEGER || tokens[index].type == IDENTIFIER || tokens[index].type == CHAR || tokens[index].type == LIST) {
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
        if (tokens[index].type != INTEGER_IDENTIFIER && tokens[index].type != CHAR_IDENTIFIER && tokens[index].type != LIST_IDENTIFIER) {
            std::cerr << "Syntax Error: Variable type needed after 'new'" << std::endl;
            exit(1);
        }
        TokenType var_type;
        if (tokens[index].type == INTEGER_IDENTIFIER) var_type = INTEGER;
        if (tokens[index].type == CHAR_IDENTIFIER) var_type = CHAR;
        if (tokens[index].type == LIST_IDENTIFIER) var_type = LIST;
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
        if (var_type == CHAR) {
            variables_char[variable_name.name] = ' ';
            variables_type[variable_name.name] = CHAR;
        }
        if (var_type == LIST) {
            variables_list[variable_name.name] = {};
            variables_type[variable_name.name] = LIST;
        }
        return new AST_NODE{ variable_name, nullptr, nullptr };
    }
    if (tokens[index].type == OUTPUT) {
        index++;
        AST_NODE* expression = parse_compare(tokens, index);
        return new AST_NODE{ Token{OUTPUT,' ', 0}, expression, nullptr };
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
        return new AST_NODE{ Token{ASSIGN, ' ', 0, {}, "="}, new AST_NODE{var_token, nullptr, nullptr}, expression };
    }
    return parse_compare(tokens, index);
}

EvaluateValue evaluate(AST_NODE* node) {
    if (node->token.type == INTEGER) {
        return { INTEGER, 0, node->token.integer, {} };
    }
    if (node->token.type == CHAR) {
        return { CHAR, node->token.character, 0, {} };
    }
    if (node->token.type == LIST) {
        return { LIST, 0, 0, node->token.list };
    }
    if (node->token.type == IDENTIFIER) {
        if (variables_type[node->token.name] == INTEGER) {
            return { INTEGER, 0, variables_integer[node->token.name], {} };
        }
        if (variables_type[node->token.name] == CHAR) {
            return { CHAR, variables_char[node->token.name], 0, {} };
        }
        if (variables_type[node->token.name] == LIST) {
            return { LIST, 0, 0, variables_list[node->token.name] };
        }
        std::cerr << "Error: Undefined variable '" << node->token.name << "'" << std::endl;
        exit(1);
    }
    if (node->token.type == OUTPUT) {
        EvaluateValue value = evaluate(node->left);
        if (value.type == CHAR) std::cout << "> " << value.character << std::endl;
        else if (value.type == INTEGER) std::cout << "> " << value.integer << std::endl;
        else if (value.type == LIST) {
            std::unordered_set<int> hash_set;
            for (auto it : value.list) {
                hash_set.insert((int)it.type);
            }
            if (hash_set.size() != 1 || *hash_set.begin() != (int)CHAR) {
                std::cerr << "Error: You cannot output the entire list in one go" << std::endl;
                exit(1);
            }
            else {
                std::cout << "> ";
                for (auto it : value.list) {
                    std::cout << it.character;
                }
            }
        }
        return value;
    }
    if (node->token.type == INPUT) {
        EvaluateValue value;
        std::string input;
        std::cin >> input;
        if (variables_type[node->token.name] == INTEGER) {
            variables_integer[node->token.name] = value.integer = stoi(input);
            value.type = INTEGER;
            value.character = 0;
            value.list = {};
        }
        if (variables_type[node->token.name] == CHAR) {
            variables_char[node->token.name] = value.character = string_to_char(input);
            value.type = CHAR;
            value.integer = 0;
            value.list = {};
        }
        if (variables_type[node->token.name] == LIST) {
            std::cerr << "Error: You cannot input the entire list in one go" << std::endl;
            exit(1);
        }
        return value;
    }
    EvaluateValue left_val = evaluate(node->left);
    if (node->token.type == ASSIGN) {
        if (node->left->token.type != IDENTIFIER) {
            std::cerr << "Error: Left side of assignment must be a variable" << std::endl;
            exit(1);
        }
        std::string var_name = node->left->token.name;
        EvaluateValue value = evaluate(node->right);
        if (variables_type[var_name] != value.type && !((variables_type[var_name] == CHAR && value.type == INTEGER) || (variables_type[var_name] == INTEGER && value.type == CHAR))) {
            std::cerr << "Error: Assign wrong value type to the variable" << std::endl;
            exit(1);
        }
        if (value.type == INTEGER) {
            if (variables_type[var_name] == CHAR) variables_char[var_name] = value.integer;
            else if (variables_type[var_name] == INTEGER) variables_integer[var_name] = value.integer;
        }
        if (value.type == CHAR) {
            if (variables_type[var_name] == CHAR) variables_char[var_name] = value.character;
            else if (variables_type[var_name] == INTEGER) variables_integer[var_name] = value.character;
        }
        if (value.type == LIST) {
            if (variables_type[var_name] == LIST) {
                variables_list[var_name] = value.list;
            }
        }
        return value;
    }
    EvaluateValue right_val = evaluate(node->right);
    if (node->token.type == GET_VALUE) {
        if (left_val.type == LIST && right_val.type != LIST) {
            int list_index = -1;
            if (right_val.type == CHAR) list_index = (int)right_val.character;
            else if (right_val.type == INTEGER) list_index = right_val.integer;
            if (list_index >= left_val.list.size()) {
                std::cerr << "Error: List out of bounds access" << std::endl;
                exit(1);
            }
            if (left_val.list[list_index].type == CHAR) {
                return { CHAR, left_val.list[list_index].character, 0, {} };
            }
            else if (left_val.list[list_index].type == INTEGER) {
                return { INTEGER, 0, left_val.list[list_index].integer, {} };
            }
            else if (left_val.list[list_index].type == LIST) {
                return { LIST, 0, 0, left_val.list[list_index].list };
            }
        }
    }
    if (node->token.type == PLUS) {
        if (left_val.type == INTEGER) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer + (int)right_val.character), {} };
            }
            else if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)(left_val.integer + right_val.integer), {} };
            }
            else if (right_val.type == LIST) {
                std::cerr << "You cannot add a list onto an integer" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == CHAR) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)((int)left_val.character + (int)right_val.character), {} };
            }
            else if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)((int)left_val.character + right_val.integer), {} };
            }
            else if (right_val.type == LIST) {
                std::cerr << "You cannot add a list onto a character" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == LIST) {
            std::vector<list_element> list = left_val.list;
            if (right_val.type == CHAR) {
                list.push_back({ 0, right_val.character });
            }
            else if (right_val.type == INTEGER) {
                list.push_back({ right_val.integer, 0 });
            }
            else if (right_val.type == LIST) {
                for (auto it : right_val.list) {
                    list.push_back(it);
                }
            }
            return { LIST, 0, 0, list };
        }
    }
    if (node->token.type == MINUS) {
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)(left_val.character - right_val.integer) };
            }
            else if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.character - right_val.character) };
            }
            else if (right_val.type == LIST) {
                std::cerr << "You cannot subtract a list from an integer" << std::endl;
                exit(1);
            }
        }
        if (left_val.type == INTEGER) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer - right_val.character) };
            }
            else if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)(left_val.integer - right_val.integer) };
            }
            else if (right_val.type == LIST) {
                std::cerr << "You cannot subtract a list from a character" << std::endl;
                exit(1);
            }
        }
        if (left_val.type == LIST) {
            std::cerr << "You cannot subtract things from a list" << std::endl;
            exit(1);
        }
    }
    if (node->token.type == MULTIPLY) {
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)(left_val.character * right_val.integer) };
            }
            else if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.character * right_val.character) };
            }
            else if (right_val.type == LIST) {
                std::cerr << "You cannot multiply an integer by a list" << std::endl;
                exit(1);
            }
        }
        if (left_val.type == INTEGER) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer * right_val.character) };
            }
            else if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)(left_val.integer * right_val.integer) };
            }
            else if (right_val.type == LIST) {
                std::cerr << "You cannot multiply a character by a list" << std::endl;
                exit(1);
            }
        }
        if (left_val.type == LIST) {
            std::cerr << "You cannot multiply the list by anything" << std::endl;
            exit(1);
        }
    }
    if (node->token.type == DIVIDE) {
        if (right_val.integer == 0 && right_val.type == INTEGER) {
            std::cerr << "Error: Division by zero" << std::endl;
            exit(1);
        }
        if (right_val.character == 0 && right_val.type == CHAR) {
            std::cerr << "Error: Division by zero" << std::endl;
            exit(1);
        }
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)(left_val.character / right_val.integer) };
            }
            else if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.character / right_val.character) };
            }
            else if (right_val.type == LIST) {
                std::cerr << "You cannot divide a character by a list" << std::endl;
                exit(1);
            }
        }
        if (left_val.type == INTEGER) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer / right_val.character) };
            }
            else if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)(left_val.integer / right_val.integer) };
            }
            else if (right_val.type == LIST) {
                std::cerr << "You cannot divide an integer by a list" << std::endl;
                exit(1);
            }
        }
        if (left_val.type == LIST) {
            std::cerr << "You cannot divide a list by anything" << std::endl;
            exit(1);
        }
    }
    if (node->token.type == MORE) {
        if (left_val.type == LIST || right_val.type == LIST) {
            std::cerr << "You cannot compare a list to anything" << std::endl;
            exit(1);
        }
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)(left_val.character > right_val.integer) };
            }
            else if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.character > right_val.character) };
            }
        }
        if (left_val.type == INTEGER) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer > right_val.character) };
            }
            else if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)(left_val.integer > right_val.integer) };
            }
        }
    }
    if (node->token.type == MORE_EQUAL) {
        if (left_val.type == LIST || right_val.type == LIST) {
            std::cerr << "You cannot compare a list to anything" << std::endl;
            exit(1);
        }
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)(left_val.character >= right_val.integer) };
            }
            else if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.character >= right_val.character) };
            }
        }
        if (left_val.type == INTEGER) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer >= right_val.character) };
            }
            else if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)(left_val.integer >= right_val.integer) };
            }
        }
    }
    if (node->token.type == LESS) {
        if (left_val.type == LIST || right_val.type == LIST) {
            std::cerr << "You cannot compare a list to anything" << std::endl;
            exit(1);
        }
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)(left_val.character < right_val.integer) };
            }
            else if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.character < right_val.character) };
            }
        }
        if (left_val.type == INTEGER) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer < right_val.character) };
            }
            else if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)(left_val.integer < right_val.integer) };
            }
        }
    }
    if (node->token.type == LESS_EQUAL) {
        if (left_val.type == LIST || right_val.type == LIST) {
            std::cerr << "You cannot compare a list to anything" << std::endl;
            exit(1);
        }
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)(left_val.character <= right_val.integer) };
            }
            else if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.character <= right_val.character) };
            }
        }
        if (left_val.type == INTEGER) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer <= right_val.character) };
            }
            else if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)(left_val.integer <= right_val.integer) };
            }
        }
    }
    if (node->token.type == EQUAL) {
        if (left_val.type == LIST || right_val.type == LIST) {
            std::cerr << "You cannot compare a list to anything" << std::endl;
            exit(1);
        }
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)(left_val.character == right_val.integer) };
            }
            else if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.character == right_val.character) };
            }
        }
        if (left_val.type == INTEGER) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer == right_val.character) };
            }
            else if (right_val.type == INTEGER) {
                return { INTEGER, 0, (int)(left_val.integer == right_val.integer) };
            }
        }
    }
    return { NONE, 0, 0 , {} };
}

void FREE_AST(AST_NODE* node) { //delete the AST =)
    if (node) {
        FREE_AST(node->left);
        FREE_AST(node->right);
        delete node;
    }
}
