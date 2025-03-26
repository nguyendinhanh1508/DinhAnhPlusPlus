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
            tokens.push_back({ NUMBER, num });
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
