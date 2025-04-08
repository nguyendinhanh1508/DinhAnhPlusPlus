#include <iostream>
#include <vector>
#include <fstream>
#include "Values.h"
#include "Tokenizer.cpp"
#include "Parser.h"
#include "Evaluate.cpp"
#include "Storage.h"

int main(int argc, char* argv[]) {
    std::ifstream file(argv[1]);
    std::string input;
    std::vector<Token> final_tokens;
    while (std::getline(file, input)) {
        if (input.empty()) continue;
        if (input == "EXIT") break;
        std::vector<Token> tokens = tokenize(input);
        if (tokens.empty()) continue;
        if (tokens.size() == 1 && tokens.back().type == END) continue;
        for(auto it : tokens) final_tokens.push_back(it);
    }
    if(final_tokens.empty()) exit(1);
    size_t index = 0;
    while(index < final_tokens.size()) {
        if(final_tokens[index].type == END) {
            index++;
            continue;
        }
        AST_NODE* cur_root = parse_language(final_tokens, index);
        EvaluateValue res = evaluate(cur_root);
        FREE_AST(cur_root);
    }
    return 0;
}
