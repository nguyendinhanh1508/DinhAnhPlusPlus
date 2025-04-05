#include <iostream>
#include <vector>
#include <cctype>
#include "Values.h"
#include "Converters.h"
#include "Storage.h"

std::vector<Token> tokenize(const std::string& input) {
    tokens.clear();
    size_t i = 0;
    while (i < input.size()) {
        if (isspace(input[i])) { //ignore spaces, we need to replace this for indentations in the future
            i++;
            continue;
        }
        else if (input[i] == ';') {
            i++;
            continue;
        }
        else if (input[i] == '[') { //check for indices
            push({ GET_VALUE, 0, 0, {}, "" });
            i++;
        }
        else if (input[i] == ']') {
            push({ INDEX_END, 0, 0, {}, "" });
            i++;
        }
        else if (input[i] == '{') { //check for lists/function body
            push({ CURLY_LEFT });
            i++;
        }
        else if (input[i] == '}') { //out of the list/function body
            if (in_list == 0 && in_function_body == 1) {
                in_function_body--;
                function_body[cur_function_name] = cur_function_body;
                cur_function_name.clear();
                cur_function_body.clear();
            }
            else push({ CURLY_RIGHT });
            i++;
        }
        else if (isdigit(input[i])) { //check for integers
            std::string num = "";
            while (i < input.size() && isdigit(input[i])) {
                num += input[i++];
            }
            push({ INTEGER, 0, stoi(num), {}, "" });
        }
        else if (input[i] == ',') {
            push({ COMMA });
            i++;
        }
        else if (input[i] == '\"') {
            std::vector<list_element> str;
            i++;
            while (i < input.size() && input[i] != '\"') {
                str.push_back({ CHAR, 0, input[i++], {} });
            }
            if (i < input.size() && input[i] == '\"') {
                push({ STRING, 0, 0, str, "" });
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
                push({ CHAR, string_to_char(str), 0, {}, "" });
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
            if (variable == "out") push({ OUTPUT, 0, 0, {}, "output" });
            else if (variable == "in") push({ INPUT, 0, 0, {}, "input" });
            else if (variable == "getline") push({ GETLINE, 0, 0, {}, "input" });
            else if (variable == "nextline") push({ CHAR, '\n', 0, {}, "" });
            else if (variable == "int") {
                push({ NEW_VAR, 0, 0, {}, "create" });
                push({ INTEGER_IDENTIFIER, 0, 0, {}, "integer" });
            }
            else if (variable == "char") {
                push({ NEW_VAR, 0, 0, {}, "create" });
                push({ CHAR_IDENTIFIER, 0, 0, {}, "character" });
            }
            else if (variable == "string") {
                push({ NEW_VAR, 0, 0, {}, "create" });
                push({ STRING_IDENTIFIER, 0, 0, {}, "string" });
            }
            else if (variable == "list") {
                push({ NEW_VAR, 0, 0, {}, "create" });
                push({ LIST_IDENTIFIER, 0, 0, {}, "list" });
            }
            else if (variable == "bool") {
                push({ NEW_VAR, 0, 0, {}, "create" });
                push({ BOOLEAN_IDENTIFIER, 0, 0, {}, "boolean" });
            }
            else if (variable == "func") {
                push({ NEW_VAR, 0, 0, {}, "create" });
                push({ FUNCTION_IDENTIFIER, 0, 0, {}, "function" });
            }
            else if (variable == "true") {
                push({ BOOLEAN, 0, 1, {}, "" });
            }
            else if (variable == "false") {
                push({ BOOLEAN, 0, 0, {}, "" });
            }
            else push({ IDENTIFIER, 0, 0, {}, variable });
        }
        else if (input[i] == '>') { //check for > and >=
            if (i < input.size() - 1 && input[i + 1] == '=') {
                i++;
                push({ MORE_EQUAL, 0, 0, {}, ">=" });
            }
            else push({ MORE, 0, 0, {}, ">" });
            i++;
        }
        else if (input[i] == '<') { //check for < and <=
            if (i < input.size() - 1 && input[i + 1] == '=') {
                i++;
                push({ LESS_EQUAL, 0, 0 , {}, "<=" });
            }
            else push({ LESS, 0, 0, {}, "<" });
            i++;
        }
        else if (input[i] == '=') { //check for == and =
            if (i < input.size() - 1 && input[i + 1] == '=') {
                i++;
                push({ EQUAL, 0, 0, {}, "==" });
            }
            else push({ ASSIGN, 0, 0, {}, "=" });
            i++;
        }
        else if (input[i] == '!') { //check for !=
            if (i < input.size() - 1 && input[i + 1] == '=') {
                i++;
                push({ NOT_EQUAL, 0, 0, {}, "!=" });
            }
            else {
                std::cerr << "Syntax Error: Stray '!' found" << std::endl;
                exit(1);
            }
            i++;
        }
        else if (input[i] == '(') { //check for (
            push({ LEFT_PARENTHESIS, 0, 0, {}, "(" });
            i++;
        }
        else if (input[i] == ')') { //check for )
            push({ RIGHT_PARENTHESIS, 0, 0, {}, ")" });
            i++;
        }
        else if (input[i] == '+') { //check for +
            push({ PLUS, 0, 0, {}, "+" });
            i++;
        }
        else if (input[i] == '-') { //check for -
            push({ MINUS, 0, 0, {}, "-" });
            i++;
        }
        else if (input[i] == '*') { //check for *
            push({ MULTIPLY, 0, 0, {}, "*" });
            i++;
        }
        else if (input[i] == '/') { //check for /
            push({ DIVIDE, 0, 0, {}, "/" });
            i++;
        }
    }
    push({ END, 0, 0, {}, "" });
    return tokens;
}
