#include <iostream>
#include <vector>
#include <cctype>
#include "DataTypes.h"
#include "Converters.h"

std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    size_t i = 0;
    std::vector<std::vector<list_element>> list;
    int in_list;
    while (i < input.size()) {
        if (isspace(input[i])) { //ignore spaces, we need to replace this for indentations in the future
            i++;
            continue;
        }
        else if (input[i] == '[') { //check for indices
            tokens.push_back({ GET_VALUE, 0, 0, {}, "" });
            i++;
        }
        else if (input[i] == ']') {
            tokens.push_back({ INDEX_END, 0, 0, {}, "" });
            i++;
        }
        else if (input[i] == '{') { //check for lists
            in_list++;
            list.push_back({});
            i++;
        }
        else if (input[i] == '}') { //out of the list
            in_list--;
            std::vector<list_element> cur_list = list.back();
            list.pop_back();
            if (list.empty()) tokens.push_back({ LIST, 0, 0, cur_list, "" });
            else list.back().push_back({ LIST, 0, 0, cur_list });
            i++;
        }
        else if (isdigit(input[i])) { //check for integers
            std::string num = "";
            while (i < input.size() && isdigit(input[i])) {
                num += input[i++];
            }
            if (in_list) list.back().push_back({ INTEGER, stoi(num), 0 });
            else tokens.push_back({ INTEGER, 0, stoi(num), {}, "" });
        }
        else if (input[i] == ',') {
            if (in_list) {
                i++;
            }
            else {
                std::cerr << "Syntax Error: Stray ',' found";
                exit(1);
            }
        }
        else if (input[i] == '\'') { //check for characters
            std::string str = "";
            i++;
            while (i < input.size() && input[i] != '\'') {
                str += input[i++];
            }
            if (str.size() > 1) {
                std::cerr << "Error: Value is not a character" << std::endl;
                exit(1);
            }
            else if (i < input.size() && input[i] == '\'') {
                if (in_list) list.back().push_back({ CHAR, 0, string_to_char(str) });
                else tokens.push_back({ CHAR, string_to_char(str), 0, {}, "" });
                i++;
            }
            else {
                std::cerr << "Error: Missing closing quote for string literal" << std::endl;
                exit(1);
            }
        }
        else if (isalpha(input[i]) || input[i] == '_') { //check for variable names
            std::string variable;
            while (i < input.size() && (isalnum(input[i]) || input[i] == '_')) {
                variable += input[i++];
            }
            if (variable == "out") tokens.push_back({ OUTPUT, 0, 0, {}, "" });
            else if (variable == "in") tokens.push_back({ INPUT, 0, 0, {}, "" });
            else if (variable == "new") tokens.push_back({ NEW_VAR, 0, 0, {}, "" });
            else if (variable == "int") tokens.push_back({ INTEGER_IDENTIFIER, 0, 0, {}, "" });
            else if (variable == "char") tokens.push_back({ CHAR_IDENTIFIER, 0, 0, {}, "" });
            else if (variable == "list") tokens.push_back({ LIST_IDENTIFIER, 0, 0, {}, "" });
            else tokens.push_back({ IDENTIFIER, 0, 0, {}, variable });
        }
        else if (input[i] == '>') { //check for > and >=
            if (i < input.size() - 1 && input[i + 1] == '=') {
                i++;
                tokens.push_back({ MORE_EQUAL, 0, 0, {}, ">=" });
            }
            else tokens.push_back({ MORE, 0, 0, {}, ">" });
            i++;
        }
        else if (input[i] == '<') { //check for < and <=
            if (i < input.size() - 1 && input[i + 1] == '=') {
                i++;
                tokens.push_back({ LESS_EQUAL, 0, 0 , {}, "<=" });
            }
            else tokens.push_back({ LESS, 0, 0, {}, "<" });
            i++;
        }
        else if (input[i] == '=') { //check for == and =
            if (i < input.size() - 1 && input[i + 1] == '=') {
                i++;
                tokens.push_back({ EQUAL, 0, 0, {}, "==" });
            }
            else tokens.push_back({ ASSIGN, 0, 0, {}, "=" });
            i++;
        }
        else if (input[i] == '(') { //check for (
            tokens.push_back({ LEFT_PARENTHESIS, 0, 0, {}, "(" });
            i++;
        }
        else if (input[i] == ')') { //check for )
            tokens.push_back({ RIGHT_PARENTHESIS, 0, 0, {}, ")" });
            i++;
        }
        else if (input[i] == '+') { //check for +
            tokens.push_back({ PLUS, 0, 0, {}, "+" });
            i++;
        }
        else if (input[i] == '-') { //check for -
            tokens.push_back({ MINUS, 0, 0, {}, "-" });
            i++;
        }
        else if (input[i] == '*') { //check for *
            tokens.push_back({ MULTIPLY, 0, 0, {}, "*" });
            i++;
        }
        else if (input[i] == '/') { //check for /
            tokens.push_back({ DIVIDE, 0, 0, {}, "/" });
            i++;
        }
    }
    tokens.push_back({ END, 0, 0, {}, "" });
    return tokens;
}
