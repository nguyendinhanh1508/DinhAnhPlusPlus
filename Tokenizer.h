#include <iostream>
#include <vector>
#include <cctype>
#include "DataTypes.h"

std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    size_t i = 0;
    while (i < input.size()) {
        if (isspace(input[i])) {
            i++;
            continue;
        }
        else if (isdigit(input[i])) {
            std::string num = "";
            while (i < input.size() && isdigit(input[i])) {
                num += input[i++];
            }
            tokens.push_back({ INTEGER, num });
        }
        else if (input[i] == '\"'){
            std::string str = "";
            i++;
            while(i < input.size() && input[i] != '\"'){
                str += input[i++];
            }
            if (i < input.size() && input[i] == '\"') {
                tokens.push_back({ STRING, str });
                i++;
            }
            else {
                std::cerr << "Error: Missing closing quote for string literal" << std::endl;
                exit(1);
            }
        }
        else if (isalpha(input[i]) || input[i] == '_') {
            std::string variable;
            while (i < input.size() && (isalnum(input[i]) || input[i] == '_')) {
                variable += input[i++];
            }
            if (variable == "out") tokens.push_back({ OUTPUT, "" });
            else if (variable == "in") tokens.push_back({ INPUT, "" });
            else if (variable == "new") tokens.push_back({ NEW_VAR, "" });
            else if (variable == "int") tokens.push_back({ INTEGER_IDENTIFIER, ""});
            else if (variable == "str") tokens.push_back({ STRING_IDENTIFIER, ""});
            else tokens.push_back({ IDENTIFIER, variable });
        }
        else if (input[i] == '>') {
            if (i < input.size() - 1 && input[i + 1] == '=') {
                tokens.push_back({ MORE_EQUAL, ">=" });
            }
            else tokens.push_back({ MORE, ">" });
            i++;
        }
        else if (input[i] == '<') {
            if (i < input.size() - 1 && input[i + 1] == '=') {
                tokens.push_back({ LESS_EQUAL, "<=" });
            }
            else tokens.push_back({ LESS, "<" });
            i++;
        }
        else if(input[i] == '=') {
            if (i < input.size() - 1 && input[i + 1] == '=') {
                tokens.push_back({ EQUAL, "==" });
            }
            else tokens.push_back({ ASSIGN, "=" });
            i++;
        }
        else if (input[i] == '(') {
            tokens.push_back({ LEFT_PARENTHESIS, "(" });
            i++;
        }
        else if (input[i] == ')') {
            tokens.push_back({ RIGHT_PARENTHESIS, ")" });
            i++;
        }
        else if (input[i] == '+') {
            tokens.push_back({ PLUS, "+" });
            i++;
        }
        else if (input[i] == '-') {
            tokens.push_back({ MINUS, "-" });
            i++;
        }
        else if (input[i] == '*') {
            tokens.push_back({ MULTIPLY, "*" });
            i++;
        }
        else if (input[i] == '/') {
            tokens.push_back({ DIVIDE, "/" });
            i++;
        }
    }
    tokens.push_back({ END, "" });
    return tokens;
}
