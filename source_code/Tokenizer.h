#include <iostream>
#include <vector>
#include <cctype>
#include "Values.h"
#include "Converters.h"

std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    size_t i = 0;
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
            tokens.push_back({ CURLY_LEFT });
            i++;
        }
        else if (input[i] == '}') { //out of the list
            tokens.push_back({ CURLY_RIGHT });
            i++;
        }
        else if (isdigit(input[i])) { //check for integers
            std::string num = "";
            while (i < input.size() && isdigit(input[i])) {
                num += input[i++];
            }
            tokens.push_back({ INTEGER, 0, stoi(num), {}, "" });
        }
        else if (input[i] == ',') {
            tokens.push_back({ COMMA });
            i++;
        }
        else if (input[i] == '\"') {
            std::vector<list_element> str;
            i++;
            while (i < input.size() && input[i] != '\"') {
                str.push_back({ CHAR, 0, input[i++], {} });
            }
            if (i < input.size() && input[i] == '\"') {
                tokens.push_back({ STRING, 0, 0, str, "" });
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
                tokens.push_back({ CHAR, string_to_char(str), 0, {}, "" });
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
            if (variable == "out") tokens.push_back({ OUTPUT, 0, 0, {}, "output" });
            else if (variable == "in") tokens.push_back({ INPUT, 0, 0, {}, "input" });
            else if (variable == "getline") tokens.push_back({ GETLINE, 0, 0, {}, "input" });
            else if (variable == "nextline") tokens.push_back({ CHAR, '\n', 0, {}, "" });
            else if (variable == "int") {
                tokens.push_back({ NEW_VAR, 0, 0, {}, "create" });
                tokens.push_back({ INTEGER_IDENTIFIER, 0, 0, {}, "integer" });
            }
            else if (variable == "char") {
                tokens.push_back({ NEW_VAR, 0, 0, {}, "create" });
                tokens.push_back({ CHAR_IDENTIFIER, 0, 0, {}, "character" });
            }
            else if (variable == "str") {
                tokens.push_back({ NEW_VAR, 0, 0, {}, "create" });
                tokens.push_back({ STRING_IDENTIFIER, 0, 0, {}, "string" });
            }
            else if (variable == "list") {
                tokens.push_back({ NEW_VAR, 0, 0, {}, "create" });
                tokens.push_back({ LIST_IDENTIFIER, 0, 0, {}, "list" });
            }
            else if (variable == "bool") {
                tokens.push_back({ NEW_VAR, 0, 0, {}, "create" });
                tokens.push_back({ BOOLEAN_IDENTIFIER, 0, 0, {}, "boolean" });
            }
            else if (variable == "true") {
                tokens.push_back({ BOOLEAN, 0, 1, {}, "" });
            }
            else if (variable == "false") {
                tokens.push_back({ BOOLEAN, 0, 0, {}, "" });
            }
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
        else if (input[i] == '!') { //check for !=
            if (i < input.size() - 1 && input[i + 1] == '=') {
                i++;
                tokens.push_back({ NOT_EQUAL, 0, 0, {}, "!=" });
            }
            else {
                std::cerr << "Syntax Error: Stray '!' found" << std::endl;
                exit(1);
            }
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
