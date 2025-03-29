#include <iostream>
#include <vector>
#include "DataTypes.h"
#include "Tokenizer.h"
#include "AST.h"

int main() {
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        if (input == "EXIT") break;
        std::vector<Token> tokens = tokenize(input);
        size_t index = 0;
        AST_NODE* root = parse_language(tokens, index);
        if (tokens[index].type != END) {
            std::cerr << "Syntax error: Line did not end properly" << std::endl;
            FREE_AST(root);
            continue;
        }
        EvaluateValue result = evaluate(root);
    }
    return 0;
}
