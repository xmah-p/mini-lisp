#include "./boot.h"

#include <fstream>
#include <iostream>

#include "./eval_env.h"
#include "./parser.h"
#include "./tokenizer.h"
#include "./value.h"

ValuePtr evaluate(std::string line) {
    auto tokens = Tokenizer::tokenize(line);
    Parser parser(std::move(tokens));
    auto value = parser.parse();
    static auto env = EvalEnv::createGlobal();
    return env->eval(std::move(value));
}

void REPLMode() {
    std::string line;
    while (true) {
        try {
            std::cout << ">>> ";
            std::getline(std::cin, line);
            if (std::cin.eof()) {
                std::exit(0);
            }
            if (line == "" || line[0] == ';') continue;
            auto result = evaluate(line);
            std::cout << result->toString() << std::endl;
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

void fileMode(std::string file) {
    if (!std::filesystem::exists(file)) {
        std::cerr << "Error: " + file + " does not exist" << std::endl;
    }
    std::ifstream src(file);
    std::string line;
    std::size_t line_num = 0;
    while (true) {
        try {
            ++line_num;
            std::getline(src, line);
            if (src.fail()) {
                return;
            }
            if (line == "" || line[0] == ';') continue;
            evaluate(line);
        } catch (std::runtime_error& e) {
            std::cerr << "Error occurred in " + file + " line " +
                             std::to_string(line_num)
                      << std::endl;
            std::cerr << line << std::endl;
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

void interactiveMode(std::string arg0, std::string arg1) {
    if (arg0 != "-i") {
        std::cerr << "Error: Unknown arguments " + arg0 << std::endl;
        std::exit(0);
    }
    fileMode(arg1);
    REPLMode();
}
