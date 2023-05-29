#include "./boot.h"

#include <fstream>
#include <iostream>

#include "./error.h"
#include "./eval_env.h"
#include "./parser.h"
#include "./reader.h"
#include "./tokenizer.h"
#include "./value.h"

ValuePtr evaluate(std::string expr) {
    auto tokens = Tokenizer::tokenize(expr);
    Parser parser(std::move(tokens));
    auto value = parser.parse();
    static auto env = EvalEnv::createGlobal();
    return env->eval(std::move(value));
}

ValuePtr readParse(std::istream& is) {
    Reader reader(is);
    std::string expr = reader.read();
    if (reader.fail()) std::exit(0);
    auto tokens = Tokenizer::tokenize(expr);
    Parser parser(std::move(tokens));
    return parser.parse();
}

void REPLMode() {
    while (true) {
        try {
            Reader reader(std::cin);
            std::string expr = reader.read();
            if (reader.fail()) std::exit(0);
            auto result = evaluate(expr);
            std::cout << result->toString() << std::endl;
        } catch (SyntaxError& e) {
            std::cerr << "SyntaxError: " << e.what() << std::endl;
        } catch (LispError& e) {
            std::cerr << "LispError: " << e.what() << std::endl;
        } catch (TypeError& e) {
            std::cerr << "TypeError: " << e.what() << std::endl;
        }
    }
}

void fileMode(const std::string& file) {
    if (!std::filesystem::exists(file)) {
        std::cerr << "Error: " + file + " does not exist" << std::endl;
        return;
    }
    std::ifstream src(file);
    std::size_t line_num = 0;
    while (true) {
        try {
            Reader reader(src, &line_num);
            std::string expr = reader.read();
            if (reader.fail()) break;
            auto result = evaluate(expr);
        } catch (std::runtime_error& e) {
            std::cerr << "Error occurred in " + file + " line " +
                             std::to_string(line_num)
                      << std::endl;
            if (typeid(e) == typeid(SyntaxError))
                std::cerr << "SyntaxError: ";
            else if (typeid(e) == typeid(LispError))
                std::cerr << "LispError: ";
            else if (typeid(e) == typeid(TypeError))
                std::cerr << "TypeError: ";
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
