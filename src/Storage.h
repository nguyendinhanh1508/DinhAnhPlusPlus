#ifndef STORAGE_H
#define STORAGE_H
#include <iostream>
#include <vector>
#include <cctype>
#include <unordered_map>
#include <unordered_set>
#include "Values.h"

int in_list = 0;
int in_function_body = 0;
std::unordered_map<std::string, int> variables_integer;
std::unordered_map<std::string, char> variables_char;
std::unordered_map<std::string, std::vector<list_element>> variables_list;
std::unordered_map<std::string, TokenType> variables_type;
std::unordered_set<std::string> already_declared;
std::unordered_map<std::string, std::vector<std::string>> function_arguments;
std::vector<Token> tokens;
std::unordered_map<std::string, std::vector<Token>> function_body;
std::vector<Token> cur_function_body;
std::string cur_function_name;

#endif