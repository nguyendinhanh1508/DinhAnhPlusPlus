#ifndef CONVERTERS_H
#define CONVERTERS_H
#include <iostream>
#include <string>
#include <vector>

std::string string_to_string(std::vector<std::string> vec){
    std::string str = "";
    for(auto it : vec){
        str += it;
    }
    return str;
}

std::vector<std::string> reverse_string_to_string(std::string str) {
    std::vector<std::string> vec;
    for (auto it : str) {
        std::string converter;
        converter.push_back(it);
        vec.push_back(converter);
    }
    return vec;
}

char string_to_char(std::string str){
    return str[0];
}
#endif