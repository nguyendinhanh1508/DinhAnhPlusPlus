#include <iostream>
#include <vector>
#include <cctype>
#include "Values.h"
#include "Converters.h"

std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    size_t i = 0;
    std::vector<std::vector<list_element>> list;
    int in_list = 0;
    while (i < input.size()) {
        if (isspace(input[i])) { //ignore spaces, we need to replace this for indentations in the future
            i++;
            continue;
        }
        else if (input[i] == ':') {
            func = false;
            in_func = true;
        }
        else if (input[i] == '[') { //check for indices
            if (in_func) cur_func_content.push_back({ GET_VALUE, 0, 0, {}, "" });
            else tokens.push_back({ GET_VALUE, 0, 0, {}, "" });
            i++;
        }
        else if (input[i] == ']') {
            if (in_func) cur_func_content.push_back({ INDEX_END, 0, 0, {}, "" });
            else tokens.push_back({ INDEX_END, 0, 0, {}, "" });
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
            if (list.empty()) {
                if (in_func) cur_func_content.push_back({ LIST, 0, 0, cur_list, "" });
                else tokens.push_back({ LIST, 0, 0, cur_list, "" });
            }
            else list.back().push_back({ LIST, 0, 0, cur_list });
            i++;
        }
        else if (isdigit(input[i])) { //check for integers
            std::string num = "";
            while (i < input.size() && isdigit(input[i])) {
                num += input[i++];
            }
            if (in_list) list.back().push_back({ INTEGER, stoi(num), 0 });
            else if (in_func) cur_func_content.push_back({ INTEGER, 0, stoi(num), {}, "" });
            else tokens.push_back({ INTEGER, 0, stoi(num), {}, "" });
        }
        else if (input[i] == ',') {
            if (in_list || in_parameter) {
                i++;
            }
            else {
                std::cerr << "Syntax Error: Stray ',' found";
                exit(1);
            }
        }
        else if (input[i] == '\"') {
            std::vector<list_element> str;
            i++;
            while (i < input.size() && input[i] != '\"') {
                str.push_back({ CHAR, 0, input[i++], {} });
            }
            if (i < input.size() && input[i] == '\"') {
                if (in_list) list.back().push_back({ STRING, 0, 0,str });
                else if (in_func) cur_func_content.push_back({ STRING, 0, 0, str, "" });
                else tokens.push_back({ STRING, 0, 0, str, "" });
                i++;
            }
            else {
                std::cerr << "Syntax Error: Missing closing quote for string lateral" << std::endl;
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
            if (i < input.size() && input[i] == '\'') {
                if (in_list) list.back().push_back({ CHAR, 0, string_to_char(str), {} });
                else if (in_func) cur_func_content.push_back({ CHAR, string_to_char(str), 0, {}, "" });
                else tokens.push_back({ CHAR, string_to_char(str), 0, {}, "" });
                i++;
            }
            else {
                std::cerr << "Syntax Error: Missing closing quote for character" << std::endl;
                exit(1);
            }
        }
        else if (isalpha(input[i]) || input[i] == '_') { //check for variable names
            std::string variable;
            while (i < input.size() && (isalnum(input[i]) || input[i] == '_')) {
                variable += input[i++];
            }
            if (variable == "out") {
                if (in_func) cur_func_content.push_back({ OUTPUT, 0, 0, {}, "output" });
                else tokens.push_back({ OUTPUT, 0, 0, {}, "output" });
            }
            else if (variable == "in") {
                if (in_func) cur_func_content.push_back({ INPUT, 0, 0, {}, "input" });
                else tokens.push_back({ INPUT, 0, 0, {}, "input" });
            }
            else if (variable == "getline") {
                if (in_func) cur_func_content.push_back({ GETLINE, 0, 0, {}, "input" });
                else tokens.push_back({ GETLINE, 0, 0, {}, "input" });
            }
            else if (variable == "nextline") {
                if (in_func) cur_func_content.push_back({ CHAR, '\n', 0, {}, "" });
                else tokens.push_back({ CHAR, '\n', 0, {}, "" });
            }
            else if (variable == "int") {
                if (in_func) {
                    cur_func_content.push_back({ NEW_VAR, 0, 0, {}, "create" });
                    cur_func_content.push_back({ INTEGER_IDENTIFIER, 0, 0, {}, "integer" });
                }
                else {
                    tokens.push_back({ NEW_VAR, 0, 0, {}, "create" });
                    tokens.push_back({ INTEGER_IDENTIFIER, 0, 0, {}, "integer" });
                }
            }
            else if (variable == "char") {
                if (in_func) {
                    cur_func_content.push_back({ NEW_VAR, 0, 0, {}, "create" });
                    cur_func_content.push_back({ CHAR_IDENTIFIER, 0, 0, {}, "character" });
                }
                else {
                    tokens.push_back({ NEW_VAR, 0, 0, {}, "create" });
                    tokens.push_back({ CHAR_IDENTIFIER, 0, 0, {}, "character" });
                }
            }
            else if (variable == "str") {
                if (in_func) {
                    cur_func_content.push_back({ NEW_VAR, 0, 0, {}, "create" });
                    cur_func_content.push_back({ STRING_IDENTIFIER, 0, 0, {}, "string" });
                }
                else {
                    tokens.push_back({ NEW_VAR, 0, 0, {}, "create" });
                    tokens.push_back({ STRING_IDENTIFIER, 0, 0, {}, "string" });
                }
            }
            else if (variable == "list") {
                if (in_func) {
                    cur_func_content.push_back({ NEW_VAR, 0, 0, {}, "create" });
                    cur_func_content.push_back({ LIST_IDENTIFIER, 0, 0, {}, "list" });
                }
                else {
                    tokens.push_back({ NEW_VAR, 0, 0, {}, "create" });
                    tokens.push_back({ LIST_IDENTIFIER, 0, 0, {}, "list" });
                }
            }
            else if (variable == "bool") {
                if (in_func) {
                    cur_func_content.push_back({ NEW_VAR, 0, 0, {}, "create" });
                    cur_func_content.push_back({ BOOLEAN_IDENTIFIER, 0, 0, {}, "boolean" });
                }
                else {
                    tokens.push_back({ NEW_VAR, 0, 0, {}, "create" });
                    tokens.push_back({ BOOLEAN_IDENTIFIER, 0, 0, {}, "boolean" });
                }
            }
            else if (variable == "true") {
                if (in_list) list.back().push_back({ BOOLEAN, 1, 0 });
                else if (in_func) cur_func_content.push_back({ BOOLEAN, 0, 1, {}, "" });
                else tokens.push_back({ BOOLEAN, 0, 1, {}, "" });
            }
            else if (variable == "false") {
                if (in_list) list.back().push_back({ BOOLEAN, 0, 0 });
                else if (in_func) cur_func_content.push_back({ BOOLEAN, 0, 0, {}, "" });
                else tokens.push_back({ BOOLEAN, 0, 0, {}, "" });
            }
            else if (variable == "func") {
                if (in_list) {
                    std::cerr << "Error: Cannot declare a function inside of a list" << std::endl;
                    exit(1);
                }
                else if (in_func) {
                    std::cerr << "Error: cannot declare a function inside of another function" << std::endl;
                    exit(1);
                }
                func = true;
                cur_func_name.clear();
                cur_func_parameter.clear();
                cur_func_content.clear();
            }
            else if (variable == "endfunc") {
                function_content[cur_func_name] = cur_func_content;
                in_func = false;
            }
            else {
                if (in_parameter) {
                    cur_func_parameter.push_back(variable);
                }
                else if (func) {
                    cur_func_name = variable;
                }
                else if (in_func) {
                    cur_func_content.push_back({ IDENTIFIER, 0, 0, {}, variable });
                }
                else {
                    tokens.push_back({ IDENTIFIER, 0, 0, {}, variable });
                }
            }
        }
        else if (input[i] == '>') { //check for > and >=
            if (i < input.size() - 1 && input[i + 1] == '=') {
                i++;
                if (in_func) cur_func_content.push_back({ MORE_EQUAL, 0, 0, {}, ">=" });
                else tokens.push_back({ MORE_EQUAL, 0, 0, {}, ">=" });
            }
            else {
                if (in_func) cur_func_content.push_back({ MORE, 0, 0, {}, ">" });
                else tokens.push_back({ MORE, 0, 0, {}, ">" });
            }
            i++;
        }
        else if (input[i] == '<') { //check for < and <=
            if (i < input.size() - 1 && input[i + 1] == '=') {
                i++;
                if (in_func) cur_func_content.push_back({ LESS_EQUAL, 0, 0 , {}, "<=" });
                else tokens.push_back({ LESS_EQUAL, 0, 0 , {}, "<=" });
            }
            else {
                if (in_func) cur_func_content.push_back({ LESS, 0, 0, {}, "<" });
                else tokens.push_back({ LESS, 0, 0, {}, "<" });
            }
            i++;
        }
        else if (input[i] == '=') { //check for == and =
            if (i < input.size() - 1 && input[i + 1] == '=') {
                i++;
                if (in_func) cur_func_content.push_back({ EQUAL, 0, 0, {}, "==" });
                else tokens.push_back({ EQUAL, 0, 0, {}, "==" });
            }
            else tokens.push_back({ ASSIGN, 0, 0, {}, "=" });
            i++;
        }
        else if (input[i] == '!') { //check for !=
            if (i < input.size() - 1 && input[i + 1] == '=') {
                i++;
                if (in_func) cur_func_content.push_back({ NOT_EQUAL, 0, 0, {}, "!=" });
                else tokens.push_back({ NOT_EQUAL, 0, 0, {}, "!=" });
            }
            else {
                std::cerr << "Syntax Error: Stray '!' found" << std::endl;
                exit(1);
            }
            i++;
        }
        else if (input[i] == '(') { //check for (
            if (func) in_parameter = true;
            else {
                if (in_func) cur_func_content.push_back({ LEFT_PARENTHESIS, 0, 0, {}, "(" });
                else tokens.push_back({ LEFT_PARENTHESIS, 0, 0, {}, "(" });
            }
            i++;
        }
        else if (input[i] == ')') { //check for )
            if (in_parameter) {
                function_parameter[cur_func_name] = cur_func_parameter;
                in_parameter = false;
            }
            else {
                if (in_func) cur_func_content.push_back({ RIGHT_PARENTHESIS, 0, 0, {}, ")" });
                else tokens.push_back({ RIGHT_PARENTHESIS, 0, 0, {}, ")" });
            }
            i++;
        }
        else if (input[i] == '+') { //check for +
            if (in_func) cur_func_content.push_back({ PLUS, 0, 0, {}, "+" });
            else tokens.push_back({ PLUS, 0, 0, {}, "+" });
            i++;
        }
        else if (input[i] == '-') { //check for -
            if (in_func) cur_func_content.push_back({ MINUS, 0, 0, {}, "-" });
            else tokens.push_back({ MINUS, 0, 0, {}, "-" });
            i++;
        }
        else if (input[i] == '*') { //check for *
            if (in_func) cur_func_content.push_back({ MULTIPLY, 0, 0, {}, "*" });
            else tokens.push_back({ MULTIPLY, 0, 0, {}, "*" });
            i++;
        }
        else if (input[i] == '/') { //check for /
            if (in_func) cur_func_content.push_back({ DIVIDE, 0, 0, {}, "/" });
            else tokens.push_back({ DIVIDE, 0, 0, {}, "/" });
            i++;
        }
    }
    if (in_func) cur_func_content.push_back({ END, 0, 0, {}, "" });
    else tokens.push_back({ END, 0, 0, {}, "" });
    return tokens;
}
