#include <iostream>
#include <vector>
#include "Converters.h"
#include "Values.h"
#include "Parser.h"
#include <string>
#include <unordered_set>
#include <unordered_map>
#include "Storage.h"
#include <algorithm>

//we should always skip END type, our code cannot handle that

EvaluateValue evaluate(AST_NODE* node) {
    if (!node) {
        return { NONE };
    }
    if (node->token.type == NEW_VAR) {
        std::string var_name = node->token.name;
        TokenType var_type = node->left->token.type;
        already_declared.insert(var_name);
        variables_type[var_name] = var_type;
        if (var_type == INTEGER || var_type == BOOLEAN) {
            variables_integer[var_name] = 0;
        }
        else if (var_type == CHAR) {
            variables_char[var_name] = 0;
        }
        else if (var_type == LIST || var_type == STRING) {
            variables_list[var_name] = {};
        }
        if (node->right) {
            EvaluateValue value = evaluate(node->right);
            if (var_type == INTEGER || var_type == BOOLEAN) {
                if (value.type == INTEGER || value.type == BOOLEAN) {
                    variables_integer[var_name] = value.integer;
                }
                else if (value.type == CHAR) {
                    variables_integer[var_name] = value.character;
                }
                else {
                    std::cerr << "Error: Mismatched types" << std::endl;
                    exit(1);
                }
            }
            else if (var_type == CHAR) {
                if (value.type == CHAR) {
                    variables_char[var_name] = value.character;
                }
                else if (value.type == INTEGER) {
                    variables_char[var_name] = value.integer;
                }
                else {
                    std::cerr << "Error: Mismatched types" << std::endl;
                    exit(1);
                }
            }
            else if (var_type == LIST || var_type == STRING) {
                if (value.type == var_type) {
                    variables_list[var_name] = value.list;
                }
                else {
                    std::cerr << "Error: Mismatched types" << std::endl;
                    exit(1);
                }
            }
        }
        return { NONE };
    }
    else if (node->token.type == INTEGER) {
        return { INTEGER, 0, node->token.integer, {} , node->token.name };
    }
    else if (node->token.type == CHAR) {
        return { CHAR, node->token.character, 0, {}, node->token.name };
    }
    else if (node->token.type == LIST) {
        std::vector<list_element> list;
        AST_NODE* cur = node->right;
        while (cur) {
            EvaluateValue evaluated_val = evaluate(cur);
            list_element list_val;
            if (evaluated_val.type == INTEGER) {
                list_val = { INTEGER, evaluated_val.integer };
            }
            else if (evaluated_val.type == CHAR) {
                list_val = { CHAR, 0, evaluated_val.character };
            }
            else if (evaluated_val.type == STRING) {
                list_val = { STRING, 0, 0, evaluated_val.list };
            }
            else if (evaluated_val.type == LIST) {
                list_val = { LIST, 0, 0, evaluated_val.list };
            }
            list.push_back(list_val);
            cur = cur->right;
        }
        return { LIST, 0, 0, list, node->token.name };
    }
    else if (node->token.type == STRING) {
        return { STRING, 0, 0, node->token.list, node->token.name };
    }
    else if (node->token.type == BOOLEAN) {
        return { BOOLEAN, 0, node->token.integer, {}, node->token.name };
    }
    else if (node->token.type == FUNCTION_CALL) {
        auto old_variables_integer = variables_integer;
        auto old_variables_char = variables_char;
        auto old_variables_list = variables_list;
        auto old_variables_type = variables_type;
        auto old_already_declared = already_declared;
        std::string func_name = node->token.name;
        if (function_body.count(func_name) == 0) {
            std::cerr << "Error: Undefined function" << std::endl;
            exit(1);
        }
        if (node->children.size() < function_arguments[func_name].size()) {
            std::cerr << "Error: Missing argument for function '" << func_name << "'" << std::endl;
            exit(1);
        }
        else if (node->children.size() > function_arguments[func_name].size()) {
            std::cerr << "Error: Arguments overload for function '" << func_name << "'" << std::endl;
            exit(1);
        }
        std::unordered_map<std::string, std::string> mutable_mapping;
        for (size_t i = 0; i < (int)node->children.size(); i++) {
            function_parameter param = function_arguments[func_name][i];
            EvaluateValue arg_val = evaluate(node->children[i]);
            if (param.type != MUTABLE) {
                already_declared.insert(param.name);
                variables_type[param.name] = arg_val.type;
                if (arg_val.type == INTEGER || arg_val.type == BOOLEAN) {
                    variables_integer[param.name] = arg_val.integer;
                }
                else if (arg_val.type == CHAR) {
                    variables_char[param.name] = arg_val.character;
                }
                else if (arg_val.type == LIST || arg_val.type == STRING) {
                    variables_list[param.name] = arg_val.list;
                }
            }
            else {
                mutable_mapping[param.name] = arg_val.name;
                variables_type[param.name] = variables_type[arg_val.name];
                if (variables_type[arg_val.name] == INTEGER || variables_type[arg_val.name] == BOOLEAN) {
                    variables_integer[param.name] = variables_integer[arg_val.name];
                }
                else if (variables_type[arg_val.name] == CHAR) {
                    variables_char[param.name] = variables_char[arg_val.name];
                }
                else if (variables_type[arg_val.name] == LIST || variables_type[arg_val.name] == STRING) {
                    variables_list[param.name] = variables_list[arg_val.name];
                }
            }
        }
        EvaluateValue res{ NONE };
        for (auto it : function_body[func_name]) {
            if (it->token.type == RETURN) {
                res = evaluate(it->left);
                break;
            }
            res = evaluate(it);
            if (res.isreturn == true) {
                break;
            }
        }
        for (auto [param_name, org_var] : mutable_mapping) {
            if (variables_type[param_name] == INTEGER || variables_type[param_name] == BOOLEAN) {
                old_variables_integer[org_var] = variables_integer[param_name];
            }
            else if (variables_type[param_name] == CHAR) {
                old_variables_char[org_var] = variables_char[param_name];
            }
            else if (variables_type[param_name] == LIST || variables_type[param_name] == STRING) {
                old_variables_list[org_var] = variables_list[param_name];
            }
        }
        for (auto it : function_global_variables[func_name]) { //assigning global variables values
            if (variables_type[it] == INTEGER || variables_type[it] == BOOLEAN) {
                old_variables_integer[it] = variables_integer[it];
            }
            else if (variables_type[it] == CHAR) {
                old_variables_char[it] = variables_char[it];
            }
            else if (variables_type[it] == LIST || variables_type[it] == STRING) {
                old_variables_list[it] = variables_list[it];
            }
        }
        variables_type = old_variables_type;
        variables_integer = old_variables_integer;
        variables_char = old_variables_char;
        variables_list = old_variables_list;
        already_declared = old_already_declared;
        return res;
    }
    else if (node->token.type == IF) {
        auto global_var = already_declared;
        auto old_variables_integer = variables_integer;
        auto old_variables_char = variables_char;
        auto old_variables_list = variables_list;
        auto old_variables_type = variables_type;
        auto old_already_declared = already_declared;
        EvaluateValue condition = evaluate(node->left);
        if (condition.type != BOOLEAN && condition.type != INTEGER && condition.type != CHAR) {
            std::cerr << "Error: If condition must be convertible to boolean" << std::endl;
            exit(1);
        }
        EvaluateValue returnval = { NONE };
        if ((condition.type == CHAR && condition.character > 0) || ((condition.type == INTEGER || condition.type == BOOLEAN) && condition.integer)) {
            for (auto it : node->children) {
                if (it->token.type == RETURN) {
                    returnval = evaluate(it->left);
                    returnval.isreturn = true;
                    break;
                }
                returnval = evaluate(it);
                if (returnval.isreturn == true) break;
            }
        }
        else if (node->right && node->right->token.type == ELSE) {
            for (auto it : node->right->children) {
                if (it->token.type == RETURN) {
                    returnval = evaluate(it->left);
                    returnval.isreturn = true;
                    break;
                }
                returnval = evaluate(it);
                if (returnval.isreturn == true) break;
            }
        }
        for (auto it : global_var) {
            if (variables_type[it] == INTEGER || variables_type[it] == BOOLEAN) {
                old_variables_integer[it] = variables_integer[it];
            }
            else if (variables_type[it] == CHAR) {
                old_variables_char[it] = variables_char[it];
            }
            else if (variables_type[it] == LIST || variables_type[it] == STRING) {
                old_variables_list[it] = variables_list[it];
            }
        }
        variables_type = old_variables_type;
        variables_integer = old_variables_integer;
        variables_char = old_variables_char;
        variables_list = old_variables_list;
        already_declared = old_already_declared;
        return returnval;
    }
    else if (node->token.type == FOR) {
        auto global_var = already_declared;
        auto old_variables_integer = variables_integer;
        auto old_variables_char = variables_char;
        auto old_variables_list = variables_list;
        auto old_variables_type = variables_type;
        auto old_already_declared = already_declared;
        if (node->children.size() > 0 && node->children[0]) {
            evaluate(node->children[0]);
        }
        EvaluateValue returnval = { NONE };
        while (true) {
            if (node->children.size() > 1 && node->children[1]) {
                EvaluateValue condition = evaluate(node->children[1]);
                if (condition.integer == 0 || condition.character) break;
            }
            bool breakloop = false;
            for (size_t i = 3; i < node->children.size(); i++) {
                if (node->children[i]) {
                    if (node->children[i]->token.type == RETURN) {
                        returnval = evaluate(node->children[i]->left);
                        returnval.isreturn = true;
                        breakloop = true;
                        break;
                    }
                    returnval = evaluate(node->children[i]);
                    if (returnval.isreturn == true) {
                        breakloop = true;
                        break;
                    }
                }
            }
            if (breakloop) break;
            if (node->children.size() > 2 && node->children[2]) {
                evaluate(node->children[2]);
            }
        }
        for (auto it : global_var) {
            if (variables_type[it] == INTEGER || variables_type[it] == BOOLEAN) {
                old_variables_integer[it] = variables_integer[it];
            }
            else if (variables_type[it] == CHAR) {
                old_variables_char[it] = variables_char[it];
            }
            else if (variables_type[it] == LIST || variables_type[it] == STRING) {
                old_variables_list[it] = variables_list[it];
            }
        }
        variables_type = old_variables_type;
        variables_integer = old_variables_integer;
        variables_char = old_variables_char;
        variables_list = old_variables_list;
        already_declared = old_already_declared;
        return returnval;
    }
    else if (node->token.type == WHILE) {
        auto global_var = already_declared;
        auto old_variables_integer = variables_integer;
        auto old_variables_char = variables_char;
        auto old_variables_list = variables_list;
        auto old_variables_type = variables_type;
        auto old_already_declared = already_declared;
        EvaluateValue returnval = { NONE };
        while (true) {
            EvaluateValue condition = evaluate(node->left);
            if (condition.type != BOOLEAN && condition.type != INTEGER && condition.type != CHAR) {
                std::cerr << "Error: While condition must be converted to bool" << std::endl;
                exit(1);
            }
            if (!condition.integer && !condition.character) {
                break;
            }
            bool breakloop = false;
            for (auto it : node->children) {
                if (it->token.type == RETURN) {
                    returnval = evaluate(it->left);
                    returnval.isreturn = true;
                    breakloop = true;
                    break;
                }
                returnval = evaluate(it);
                if (returnval.isreturn == true) {
                    breakloop = true;
                    break;
                }
            }
            if (breakloop) break;
        }
        for (auto it : global_var) {
            if (variables_type[it] == INTEGER || variables_type[it] == BOOLEAN) {
                old_variables_integer[it] = variables_integer[it];
            }
            else if (variables_type[it] == CHAR) {
                old_variables_char[it] = variables_char[it];
            }
            else if (variables_type[it] == LIST || variables_type[it] == STRING) {
                old_variables_list[it] = variables_list[it];
            }
        }
        variables_type = old_variables_type;
        variables_integer = old_variables_integer;
        variables_char = old_variables_char;
        variables_list = old_variables_list;
        already_declared = old_already_declared;
        return returnval;
    }
    if (node->token.type == IDENTIFIER) {
        if (variables_type[node->token.name] == INTEGER) {
            EvaluateValue cur = { INTEGER, 0, variables_integer[node->token.name], {}, node->token.name };
            cur.pointer.integer = &variables_integer[node->token.name];
            return cur;
        }
        else if (variables_type[node->token.name] == CHAR) {
            EvaluateValue cur = { CHAR, variables_char[node->token.name], 0, {}, node->token.name };
            cur.pointer.character = &variables_char[node->token.name];
            return cur;
        }
        else if (variables_type[node->token.name] == LIST) {
            EvaluateValue cur = { LIST, 0, 0, variables_list[node->token.name], node->token.name };
            cur.pointer.list = &variables_list[node->token.name];
            return cur;
        }
        else if (variables_type[node->token.name] == STRING) {
            EvaluateValue cur = { STRING, 0, 0, variables_list[node->token.name], node->token.name };
            cur.pointer.list = &variables_list[node->token.name];
            return cur;
        }
        else if (variables_type[node->token.name] == BOOLEAN) {
            EvaluateValue cur = { BOOLEAN, 0, variables_integer[node->token.name], {}, node->token.name };
            cur.pointer.integer = &variables_integer[node->token.name];
            return cur;
        }
        std::cerr << "Error: Undefined variable '" << node->token.name << "'" << std::endl;
        exit(1);
    }
    if (node->token.type == OUTPUT) {
        EvaluateValue value = { NONE };
        if (node->left) value = evaluate(node->left);
        if (value.type == CHAR) std::cout << value.character;
        else if (value.type == INTEGER || value.type == BOOLEAN) std::cout << value.integer;
        else if (value.type == STRING) {
            for (auto it : value.list) {
                std::cout << it.character;
            }
        }
        else if (value.type == LIST) {
            std::cerr << "Error: You cannot output the entire list in one go" << std::endl;
            exit(1);
        }
        return { NONE, 0, 0, {}, "" };
    }
    if (node->token.type == INPUT) {
        if (!node->left || node->left->token.type != IDENTIFIER) {
            std::cerr << "Error: 'in' must be followed by a variable name" << std::endl;
            exit(1);
        }
        std::string var_name = node->left->token.name;
        if (variables_type.count(var_name) == 0) {
            std::cerr << "Error: Undeclared variable '" << var_name << '\'' << std::endl;
            exit(1);
        }
        std::string input;
        std::cin >> input;
        if (variables_type[var_name] == INTEGER) {
            variables_integer[var_name] = stoi(input);
        }
        else if (variables_type[var_name] == BOOLEAN) {
            variables_integer[var_name] = (stoi(input) > 0);
        }
        else if (variables_type[var_name] == CHAR) {
            variables_char[var_name] = string_to_char(input);
        }
        else if (variables_type[var_name] == STRING) {
            variables_list[var_name] = string_to_list(input);
        }
        else if (variables_type[var_name] == LIST) {
            std::cerr << "Error: You cannot input the entire list in one go" << std::endl;
            exit(1);
        }
        return { NONE, 0, 0, {}, "" };
    }
    if (node->token.type == GETLINE) {
        if (!node->left || node->left->token.type != IDENTIFIER) {
            std::cerr << "Error: 'in' must be followed by a variable name" << std::endl;
            exit(1);
        }
        std::string var_name = node->left->token.name;
        if (variables_type.count(var_name) == 0) {
            std::cerr << "Error: Undeclared variable '" << var_name << '\'' << std::endl;
            exit(1);
        }
        std::string input;
        std::getline(std::cin, input);
        if (variables_type[var_name] == STRING) {
            variables_list[var_name] = string_to_list(input);
        }
        else {
            std::cerr << "Error: You cannot use getline for non-string values" << std::endl;
            exit(1);
        }
        return { NONE, 0, 0, {}, "" };
    }
    EvaluateValue left_val = evaluate(node->left);
    if (node->token.type == ASSIGN) {
        if (node->left->token.type != IDENTIFIER && !left_val.islist_element) {
            std::cerr << "Error: Left side of assignment must be a variable" << std::endl;
            exit(1);
        }
        std::string var_name = node->left->token.name;
        EvaluateValue value = evaluate(node->right);
        if (left_val.islist_element) {
            if (value.type == INTEGER || value.type == BOOLEAN) {
                variables_list[left_val.name][left_val.index] = {INTEGER, value.integer};
            }
            else if (value.type == CHAR) {
                variables_list[left_val.name][left_val.index] = {CHAR, 0, value.character};
            }
            else if (value.type == LIST) {
                variables_list[left_val.name][left_val.index] = {LIST, 0, 0, value.list};
            }
            else if (value.type == STRING) {
                variables_list[left_val.name][left_val.index] = {STRING, 0, 0, value.list};
            }
            return value;
        }
        else {
            if (value.type == INTEGER || value.type == BOOLEAN) {
                if (left_val.type == CHAR) *left_val.pointer.character = value.integer;
                else if (left_val.type == INTEGER) *left_val.pointer.integer = value.integer;
                else if (left_val.type == BOOLEAN) *left_val.pointer.integer = (value.integer > 0);
                else {
                    std::cerr << "Error: Assign wrong value type to the variable" << std::endl;
                    exit(1);
                }
            }
            else if (value.type == CHAR) {
                if (left_val.type == CHAR) *left_val.pointer.character = value.character;
                else if (left_val.type == INTEGER) *left_val.pointer.integer = value.character;
                else if (left_val.type == BOOLEAN) *left_val.pointer.integer = (value.character > 0);
                else {
                    std::cerr << "Error: Assign wrong value type to the variable" << std::endl;
                    exit(1);
                }
            }
            else if (value.type == LIST) {
                if (left_val.type == LIST) {
                    *left_val.pointer.list = value.list;
                }
                else {
                    std::cerr << "Error: Assign wrong value type to the variable" << std::endl;
                    exit(1);
                }
            }
            else if (value.type == STRING) {
                if (left_val.type == STRING) {
                    *left_val.pointer.list = value.list;
                }
                else {
                    std::cerr << "Error: Assign wrong value type to the variable" << std::endl;
                    exit(1);
                }
            }
            return value;
        }
    }
    EvaluateValue right_val = evaluate(node->right);
    if (node->token.type == GET_VALUE) {
        if ((left_val.type == LIST || left_val.type == STRING) && right_val.type != LIST && right_val.type != STRING) {
            int list_index = -1;
            if (right_val.type == CHAR) list_index = (int)right_val.character;
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) list_index = right_val.integer;
            if (list_index >= left_val.list.size()) {
                std::cerr << "Error: List out of bounds access" << std::endl;
                exit(1);
            }
            if (left_val.list[list_index].type == CHAR) {
                EvaluateValue cur = { CHAR, left_val.list[list_index].character, 0, {}, left_val.name, list_index };
                cur.islist_element = true;
                cur.pointer.list_element = &left_val.list[list_index];
                return cur;
            }
            else if (left_val.list[list_index].type == INTEGER) {
                EvaluateValue cur = { INTEGER, 0, left_val.list[list_index].integer, {}, left_val.name, list_index };
                cur.islist_element = true;
                cur.pointer.list_element = &left_val.list[list_index];
                return cur;
            }
            else if (left_val.list[list_index].type == LIST) {
                EvaluateValue cur = { LIST, 0, 0, left_val.list[list_index].list, left_val.name, list_index };
                cur.islist_element = true;
                cur.pointer.list_element = &left_val.list[list_index];
                return cur;
            }
            else if (left_val.list[list_index].type == STRING) {
                EvaluateValue cur = { STRING, 0, 0, left_val.list[list_index].list, left_val.name, list_index };
                cur.islist_element = true;
                cur.pointer.list_element = &left_val.list[list_index];
                return cur;
            }
            else if (left_val.list[list_index].type == BOOLEAN) {
                EvaluateValue cur = { BOOLEAN, 0, left_val.list[list_index].integer, {}, left_val.name, list_index };
                cur.islist_element = true;
                cur.pointer.list_element = &left_val.list[list_index];
                return cur;
            }
        }
        else {
            std::cerr << "Error: List index cannot be a list or a string" << std::endl;
            exit(1);
        }
    }
    if (node->token.type == AND) {
        if (left_val.type == INTEGER || left_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer & (int)right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)(left_val.integer & right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == CHAR) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)((int)left_val.character & (int)right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)((int)left_val.character & right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == LIST) {
            std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
            exit(1);
        }
        else if (left_val.type == STRING) {
            std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
            exit(1);
        }
    }
    if (node->token.type == OR) {
        if (left_val.type == INTEGER || left_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer | (int)right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)(left_val.integer | right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == CHAR) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)((int)left_val.character | (int)right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)((int)left_val.character | right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == LIST) {
            std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
            exit(1);
        }
        else if (left_val.type == STRING) {
            std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
            exit(1);
        }
    }
    if (node->token.type == XOR) {
        if (left_val.type == INTEGER || left_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer ^ (int)right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)(left_val.integer ^ right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == CHAR) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)((int)left_val.character ^ (int)right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)((int)left_val.character ^ right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == LIST) {
            std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
            exit(1);
        }
        else if (left_val.type == STRING) {
            std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
            exit(1);
        }
    }
    if (node->token.type == LEFT_SHIFT) {
        if (left_val.type == INTEGER || left_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer << (int)right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)(left_val.integer << right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == CHAR) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)((int)left_val.character << (int)right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)((int)left_val.character << right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == LIST) {
            std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
            exit(1);
        }
        else if (left_val.type == STRING) {
            std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
            exit(1);
        }
    }
    if (node->token.type == RIGHT_SHIFT) {
        if (left_val.type == INTEGER || left_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer >> (int)right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)(left_val.integer >> right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == CHAR) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)((int)left_val.character >> (int)right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)((int)left_val.character >> right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == LIST) {
            std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
            exit(1);
        }
        else if (left_val.type == STRING) {
            std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
            exit(1);
        }
    }
    if (node->token.type == AND_BOOL) {
        if (left_val.type == INTEGER || left_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer && (int)right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)(left_val.integer && right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == CHAR) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)((int)left_val.character && (int)right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)((int)left_val.character && right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == LIST) {
            std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
            exit(1);
        }
        else if (left_val.type == STRING) {
            std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
            exit(1);
        }
    }
    if (node->token.type == OR_BOOL) {
        if (left_val.type == INTEGER || left_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer || (int)right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)(left_val.integer || right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == CHAR) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)((int)left_val.character || (int)right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)((int)left_val.character || right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == LIST) {
            std::cerr << "Error: You cannot perform bitwise operations on a list" << std::endl;
            exit(1);
        }
        else if (left_val.type == STRING) {
            std::cerr << "Error: You cannot perform bitwise operations on a string" << std::endl;
            exit(1);
        }
    }
    if (node->token.type == PLUS) {
        if (left_val.type == INTEGER || left_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer + (int)right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)(left_val.integer + right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot add a list onto an integer" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot add a string onto an integer" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == CHAR) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)((int)left_val.character + (int)right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)((int)left_val.character + right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot add a list onto a character" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot add a string onto a character" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == LIST) {
            std::vector<list_element> list = left_val.list;
            if (right_val.type == CHAR) {
                list.push_back({ CHAR, 0, right_val.character, {} });
            }
            else if (right_val.type == INTEGER) {
                list.push_back({ INTEGER, right_val.integer, 0, {} });
            }
            else if (right_val.type == BOOLEAN) {
                list.push_back({ BOOLEAN, right_val.integer, 0, {} });
            }
            else if (right_val.type == LIST) {
                list.push_back({ LIST, 0, 0, right_val.list });
            }
            else if (right_val.type == STRING) {
                list.push_back({ STRING, 0, 0, right_val.list });
            }
            return { LIST, 0, 0, list, "" };
        }
        else if (left_val.type == STRING) {
            std::vector<list_element> str = left_val.list;
            if (right_val.type == CHAR) {
                str.push_back({ CHAR, 0, right_val.character, {} });
            }
            else if (right_val.type == INTEGER) {
                std::cerr << "Error: You cannot add an integer onto a string" << std::endl;
                exit(1);
            }
            else if (right_val.type == BOOLEAN) {
                std::cerr << "Error: You cannot add a boolean value onto a string" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                for (auto it : right_val.list) {
                    str.push_back(it);
                }
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot add a list onto a string" << std::endl;
                exit(1);
            }
            return { STRING, 0, 0, str, "" };
        }
    }
    if (node->token.type == MINUS) {
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)(left_val.character - right_val.integer), {}, "" };
            }
            else if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.character - right_val.character), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot subtract a list from an integer" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot subtract a string from an integer" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == INTEGER || left_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer - right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)(left_val.integer - right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot subtract a list from a character" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot subtract a string from a character" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == LIST) {
            std::cerr << "Error: You cannot subtract from a list" << std::endl;
            exit(1);
        }
        else if (left_val.type == LIST) {
            std::cerr << "Error: You cannot subtract from a string" << std::endl;
            exit(1);
        }
    }
    if (node->token.type == MULTIPLY) {
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)(left_val.character * right_val.integer), {}, "" };
            }
            else if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.character * right_val.character), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot multiply an integer by a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot multiply an integer by a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == INTEGER || left_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer * right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || left_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)(left_val.integer * right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot multiply a character by a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot multiply a character by a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == LIST) {
            std::cerr << "Error: You cannot multiply the list" << std::endl;
            exit(1);
        }
        else if (left_val.type == STRING) {
            std::cerr << "Error: You cannot multiply the string" << std::endl;
            exit(1);
        }
    }
    if (node->token.type == DIVIDE) {
        if (right_val.integer == 0 && (right_val.type == INTEGER || right_val.type == BOOLEAN)) {
            std::cerr << "Error: Division by zero" << std::endl;
            exit(1);
        }
        if (right_val.character == 0 && right_val.type == CHAR) {
            std::cerr << "Error: Division by zero" << std::endl;
            exit(1);
        }
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)(left_val.character / right_val.integer), {}, "" };
            }
            else if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.character / right_val.character), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot divide a character by a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot divide a character by a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == INTEGER || left_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer / right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)(left_val.integer / right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot divide an integer by a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot divide an integer by a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == LIST) {
            std::cerr << "Error: You cannot divide a list" << std::endl;
            exit(1);
        }
        else if (left_val.type == STRING) {
            std::cerr << "Error: You cannot divide a string" << std::endl;
            exit(1);
        }
    }
    if (node->token.type == MODULO) {
        if (right_val.integer == 0 && (right_val.type == INTEGER || right_val.type == BOOLEAN)) {
            std::cerr << "Error: Division by zero" << std::endl;
            exit(1);
        }
        if (right_val.character == 0 && right_val.type == CHAR) {
            std::cerr << "Error: Division by zero" << std::endl;
            exit(1);
        }
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)(left_val.character % right_val.integer), {}, "" };
            }
            else if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.character % right_val.character), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot modulo a character by a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot modulo a character by a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == INTEGER || left_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { INTEGER, 0, (int)(left_val.integer % right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { INTEGER, 0, (int)(left_val.integer % right_val.integer), {}, "" };
            }
            else if (right_val.type == LIST) {
                std::cerr << "Error: You cannot modulo an integer by a list" << std::endl;
                exit(1);
            }
            else if (right_val.type == STRING) {
                std::cerr << "Error: You cannot modulo an integer by a string" << std::endl;
                exit(1);
            }
        }
        else if (left_val.type == LIST) {
            std::cerr << "Error: You cannot mod a list" << std::endl;
            exit(1);
        }
        else if (left_val.type == STRING) {
            std::cerr << "Error: You cannot mod a string" << std::endl;
            exit(1);
        }
    }
    if (node->token.type == MORE) {
        if (left_val.type == LIST || right_val.type == LIST) {
            std::cerr << "Error: You cannot compare lists" << std::endl;
            exit(1);
        }
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { BOOLEAN, 0, (int)(left_val.character > right_val.integer), {}, "" };
            }
            else if (right_val.type == CHAR) {
                return { BOOLEAN, 0, (int)(left_val.character > right_val.character), {}, "" };
            }
        }
        else if (left_val.type == INTEGER || left_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { BOOLEAN, 0, (int)(left_val.integer > right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { BOOLEAN, 0, (int)(left_val.integer > right_val.integer), {}, "" };
            }
        }
        else if (left_val.type == STRING) {
            if (right_val.type == STRING) {
                return { BOOLEAN, 0, (int)(list_to_string(left_val.list) > list_to_string(right_val.list)), {}, "" };
            }
            else {
                std::cerr << "Error: Cannot compare non-string values to a string" << std::endl;
                exit(1);
            }
        }
    }
    if (node->token.type == MORE_EQUAL) {
        if (left_val.type == LIST || right_val.type == LIST) {
            std::cerr << "Error: You cannot compare lists" << std::endl;
            exit(1);
        }
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { BOOLEAN, 0, (int)(left_val.character >= right_val.integer), {}, "" };
            }
            else if (right_val.type == CHAR) {
                return { BOOLEAN, 0, (int)(left_val.character >= right_val.character), {}, "" };
            }
        }
        else if (left_val.type == INTEGER || right_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { BOOLEAN, 0, (int)(left_val.integer >= right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER) {
                return { BOOLEAN, 0, (int)(left_val.integer >= right_val.integer), {}, "" };
            }
        }
        else if (left_val.type == STRING) {
            if (right_val.type == STRING) {
                return { BOOLEAN, 0, (int)(list_to_string(left_val.list) >= list_to_string(right_val.list)), {}, "" };
            }
            else {
                std::cerr << "Error: Cannot compare non-string values to a string" << std::endl;
                exit(1);
            }
        }
    }
    if (node->token.type == LESS) {
        if (left_val.type == LIST || right_val.type == LIST) {
            std::cerr << "Error: You cannot compare lists" << std::endl;
            exit(1);
        }
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { BOOLEAN, 0, (int)(left_val.character < right_val.integer), {}, "" };
            }
            else if (right_val.type == CHAR) {
                return { BOOLEAN, 0, (int)(left_val.character < right_val.character), {}, "" };
            }
        }
        else if (left_val.type == INTEGER || left_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { BOOLEAN, 0, (int)(left_val.integer < right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER) {
                return { BOOLEAN, 0, (int)(left_val.integer < right_val.integer), {}, "" };
            }
        }
        else if (left_val.type == STRING) {
            if (right_val.type == STRING) {
                return { BOOLEAN, 0, (int)(list_to_string(left_val.list) < list_to_string(right_val.list)), {}, "" };
            }
            else {
                std::cerr << "Error: Cannot compare non-string values to a string" << std::endl;
                exit(1);
            }
        }
    }
    if (node->token.type == LESS_EQUAL) {
        if (left_val.type == LIST || right_val.type == LIST) {
            std::cerr << "Error: You cannot compare a list to anything" << std::endl;
            exit(1);
        }
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { BOOLEAN, 0, (int)(left_val.character <= right_val.integer), {}, "" };
            }
            else if (right_val.type == CHAR) {
                return { BOOLEAN, 0, (int)(left_val.character <= right_val.character), {}, "" };
            }
        }
        else if (left_val.type == INTEGER || left_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { BOOLEAN, 0, (int)(left_val.integer <= right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { BOOLEAN, 0, (int)(left_val.integer <= right_val.integer), {}, "" };
            }
        }
        else if (left_val.type == STRING) {
            if (right_val.type == STRING) {
                return { BOOLEAN, 0, (int)(list_to_string(left_val.list) <= list_to_string(right_val.list)), {}, "" };
            }
            else {
                std::cerr << "Error: Cannot compare non-string values to a string" << std::endl;
                exit(1);
            }
        }
    }
    if (node->token.type == EQUAL) {
        if (left_val.type == LIST || right_val.type == LIST) {
            std::cerr << "Error: You cannot compare lists" << std::endl;
            exit(1);
        }
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { BOOLEAN, 0, (int)(left_val.character == right_val.integer), {}, "" };
            }
            else if (right_val.type == CHAR) {
                return { BOOLEAN, 0, (int)(left_val.character == right_val.character), {}, "" };
            }
        }
        else if (left_val.type == INTEGER || left_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { BOOLEAN, 0, (int)(left_val.integer == right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER || left_val.type == BOOLEAN) {
                return { BOOLEAN, 0, (int)(left_val.integer == right_val.integer), {}, "" };
            }
        }
        else if (left_val.type == STRING) {
            if (right_val.type == STRING) {
                return { BOOLEAN, 0, (int)(list_to_string(left_val.list) == list_to_string(right_val.list)), {}, "" };
            }
            else {
                std::cerr << "Error: Cannot compare non-string values to a string" << std::endl;
                exit(1);
            }
        }
    }
    if (node->token.type == NOT_EQUAL) {
        if (left_val.type == LIST || right_val.type == LIST) {
            std::cerr << "Error: You cannot compare lists" << std::endl;
            exit(1);
        }
        if (left_val.type == CHAR) {
            if (right_val.type == INTEGER || right_val.type == BOOLEAN) {
                return { BOOLEAN, 0, (int)(left_val.character != right_val.integer), {}, "" };
            }
            else if (right_val.type == CHAR) {
                return { BOOLEAN, 0, (int)(left_val.character != right_val.character), {}, "" };
            }
        }
        else if (left_val.type == INTEGER || left_val.type == BOOLEAN) {
            if (right_val.type == CHAR) {
                return { BOOLEAN, 0, (int)(left_val.integer != right_val.character), {}, "" };
            }
            else if (right_val.type == INTEGER) {
                return { BOOLEAN, 0, (int)(left_val.integer != right_val.integer), {}, "" };
            }
        }
        else if (left_val.type == STRING) {
            if (right_val.type == STRING) {
                return { BOOLEAN, 0, (int)(list_to_string(left_val.list) != list_to_string(right_val.list)), {}, "" };
            }
            else {
                std::cerr << "Error: Cannot compare non-string values to a string" << std::endl;
                exit(1);
            }
        }
    }
    return { NONE, 0, 0 , {} };
}
