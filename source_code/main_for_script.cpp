#include <iostream>
#include <vector>
#include <fstream>
#include "Values.h"
#include "Tokenizer.cpp"
#include "Parser.h"
#include "Evaluate.cpp"

int main(int argc, char* argv[]) {
    std::ifstream file(argv[1]);
    std::string input;
    while (std::getline(file, input)) {
        if (input.empty()) continue;
        if (input == "EXIT") break;
        std::vector<Token> tokens = tokenize(input);
        if (tokens.empty()) continue;
        if (tokens.size() == 1 && tokens.back().type == END) continue;
        size_t index = 0;
        AST_NODE* root = parse_language(tokens, index);
        if (tokens[index].type != END) {
            std::cerr << "Syntax Error: Failed to parse code" << std::endl;
            FREE_AST(root);
            continue;
        }
        EvaluateValue result = evaluate(root);
        FREE_AST(root);
    }
    return 0;
}
