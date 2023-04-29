#include "./boot.h"

#include <fstream>
#include <iostream>

#include "./error.h"
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
        } catch (SyntaxError& e) {
            std::cerr << "SyntaxError: " << e.what() << std::endl;
        } catch (LispError& e) {
            std::cerr << "LispError: " << e.what() << std::endl;
        }
    }
}

void fileMode(const std::string& file) {
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
            if (dynamic_cast<SyntaxError*>(&e))
                std::cerr << "SyntaxError: ";
            else if (dynamic_cast<LispError*>(&e))
                std::cerr << "LispError: ";
            std::cerr << e.what() << std::endl;
            std::exit(0);
        }
    }
}

void interactiveMode(const std::string& opt, const std::string& file) {
    if (opt != "-i") {
        std::cerr << "Error: Unknown arguments " + opt << std::endl;
        std::exit(0);
    }
    fileMode(file);
    REPLMode();
}
