#include <iostream>
#include <string>

#include "./boot.h"
#include "./eval_env.h"
#include "./parser.h"
#include "./tokenizer.h"
#include "./value.h"
#include "rjsj_test.hpp"

struct TestCtx {
    std::string eval(std::string input) {
        auto tokens = Tokenizer::tokenize(input);
        Parser parser(std::move(tokens));
        auto value = parser.parse();
        static auto env = EvalEnv::createGlobal();
        auto result = env->eval(std::move(value));
        return result->toString();
    }
};

int test() {
    RJSJ_TEST(TestCtx, Lv2, Lv3, Lv4, Lv5, Lv5Extra, Lv6, Lv7, Lv7Lib, Sicp);
    return 0;
}

int main(int argc, char **argv) {
    // return test();

    switch (argc) {
        case 1: REPLMode(); break;
        case 2: fileMode(argv[1]); break;
        case 3: interactiveMode(argv[1], argv[2]); break;
        default: std::cerr << "Error: Invalid arguments" << std::endl;
    }
}
