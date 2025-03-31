#ifndef CONVERTERS_H
#define CONVERTERS_H
#include <iostream>
#include <string>
#include <vector>
#include "DataTypes.h"

std::vector<list_element> string_to_list(std::string str) {
    std::vector<list_element> vec;
    for (auto it : str) {
        std::string converter;
        converter.push_back(it);
        vec.push_back({ CHAR, 0, it, {} });
    }
    return vec;
}

std::string list_to_string(std::vector<list_element> list) {
    std::string str;
    for (auto it : list) {
        str += it.character;
    }
    return str;
}

char string_to_char(std::string str) {
    return str[0];
}
#endif
