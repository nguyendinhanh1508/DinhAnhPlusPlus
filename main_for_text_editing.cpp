#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "DataTypes.h"
#include "Tokenizer.h"
#include "AST.h"

//i still dunno how to do this, leave this for later

std::string read_file(const std::string& text_file) {
    std::ifstream file(text_file);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    std::string text_file = "program.dpp";
    std::string input = read_file(text_file);
    std::vector<Token> tokens = tokenize(input);
    size_t index = 0;
    AST_NODE* root = parse_language(tokens, index);
    int result = evaluate(root);
    return 0;
}