#include <iostream>
#include <vector>
#include <cctype>
#include <unordered_map>

enum TokenType { NUMBER, PLUS, MINUS, MULTIPLY, DIVIDE, ASSIGN, IDENTIFIER, END, OUTPUT, INPUT, NEW_VAR};

struct Token {
    TokenType type;
    std::string value;
};

std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    for (size_t i = 0; i < input.size(); i++) {
        if (isdigit(input[i])) {
            std::string num = "";
            while (i < input.size() && isdigit(input[i])) {
                num += input[i++];
            }
            tokens.push_back({ NUMBER, num });
            continue;
        }
        else if (isalpha(input[i])) {
            std::string variable;
            while (i < input.size() && isalnum(input[i])) {
                variable += input[i++];
            }
            if (variable == "out") tokens.push_back({ OUTPUT, "" });
            else if (variable == "in") tokens.push_back({ INPUT, "" });
            else if (variable == "create") tokens.push_back({ NEW_VAR, "" });
            else tokens.push_back({ IDENTIFIER, variable });
        }
        else if (input[i] == '+') {
            tokens.push_back({ PLUS, "+" });
        }
        else if (input[i] == '-') {
            tokens.push_back({ MINUS, "-" });
        }
        else if (input[i] == '*') {
            tokens.push_back({ MULTIPLY, "*" });
        }
        else if (input[i] == '/') {
            tokens.push_back({ DIVIDE, "/" });
        }
        else if (input[i] == '=') {
            tokens.push_back({ ASSIGN, "=" });
        }
    }
    tokens.push_back({ END, "" });
    return tokens;
}

struct AST_NODE {
    Token token;
    AST_NODE* left;
    AST_NODE* right;
};

std::unordered_map<std::string, int> variables;

AST_NODE* parse(std::vector<Token>& tokens, size_t& index) {
    if (tokens[index].type == NUMBER || tokens[index].type == IDENTIFIER) {
        AST_NODE* node = new AST_NODE{ tokens[index], nullptr, nullptr };
        index++;
        return node;
    }
    return nullptr;
}

AST_NODE* parse_higher(std::vector<Token>& tokens, size_t& index) {
    AST_NODE* left = parse(tokens, index);
    while (tokens[index].type == MULTIPLY || tokens[index].type == DIVIDE) {
        Token operation = tokens[index++];
        AST_NODE* right = parse(tokens, index);
        AST_NODE* node = new AST_NODE{ operation, left, right };
        left = node;
    }
    return left;
}

AST_NODE* parse_lower(std::vector<Token>& tokens, size_t& index) {
    AST_NODE* left = parse_higher(tokens, index);
    while (tokens[index].type == PLUS || tokens[index].type == MINUS) {
        Token operation = tokens[index++];
        AST_NODE* right = parse_higher(tokens, index);
        AST_NODE* node = new AST_NODE{ operation, left, right };
        left = node;
    }
    return left;
}

AST_NODE* parse_language(std::vector<Token>& tokens, size_t& index) {
    if (tokens[index].type == NEW_VAR) {  // create <var>
        index++;
        if (tokens[index].type != IDENTIFIER) {
            std::cerr << "Syntax Error: Expected variable after 'create'" << std::endl;
            exit(1);
        }
        Token variable_name = tokens[index++];
        variables[variable_name.value] = 0;  // Initialize variable to 0
        return new AST_NODE{variable_name, nullptr, nullptr};
    }

    if (tokens[index].type == OUTPUT) {  // out <expression>
        index++;
        AST_NODE* expression = parse_lower(tokens, index);
        return new AST_NODE{Token{OUTPUT, ""}, expression, nullptr};
    }

    if (tokens[index].type == INPUT) {  // in <variable>
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
        index += 2; // Skip identifier and '='
        AST_NODE* expr = parse_lower(tokens, index);
        // Create an assignment node with ASSIGN token, variable as left, expression as right
        return new AST_NODE{Token{ASSIGN, "="}, new AST_NODE{var_token, nullptr, nullptr}, expr};
    }

    return parse_lower(tokens, index);
}

bool division_by_zero = false;

int evaluate(AST_NODE* node) {
    if (node->token.type == NUMBER) {
        return std::stoi(node->token.value);
    }

    if (node->token.type == IDENTIFIER) {
        if (variables.count(node->token.value)) {
            return variables[node->token.value];
        }
        std::cerr << "Error: Undefined variable '" << node->token.value << "'" << std::endl;
        // exit(1);
    }

    if (node->token.type == OUTPUT) {
        int value = evaluate(node->left);
        std::cout << value << std::endl;  // Output the evaluated value
        return value;
    }

    if (node->token.type == INPUT) {
        int value;
        std::cin >> value;  // Read input value
        variables[node->token.value] = value;  // Store it in the variable
        return value;
    }

    int left_val = evaluate(node->left);
    if (node->token.type == ASSIGN) {
        if (node->left->token.type != IDENTIFIER) {
            std::cerr << "Error: Left side of assignment must be a variable" << std::endl;
            // exit(1);
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

void FREE_AST(AST_NODE* node) {
    if (node) {
        FREE_AST(node->left);
        FREE_AST(node->right);
        delete node;
    }
}

int main() {
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        if (input == "EXIT") break;
        std::vector<Token> tokens = tokenize(input);
        size_t index = 0;
        AST_NODE* root = parse_language(tokens, index);
        if (tokens[index].type != END) {
            std::cerr << "Syntax error: Line did not end properly" << std::endl;
            FREE_AST(root);
            continue;
        }
        int result = evaluate(root);
    }
    for(auto it : variables){
        std::cout << it.first << ' ' << it.second << std::endl;
    }
}
