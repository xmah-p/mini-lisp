#include "./builtins.h"

#include <iostream>
#include <memory>
#include <string>

#include "./error.h"
#include "./eval_env.h"

ValuePtr Builtins::add(const std::vector<ValuePtr>& params) {
    double total{0};
    for (auto& i : params) {
        auto num = i->asNumber();
        if (num == std::nullopt)
            throw LispError("Cannot add a non-numeric value.");
        total += *num;
    }
    return std::make_shared<NumericValue>(total);
}

void Builtins::exit(const std::vector<ValuePtr>& params) {
    if (params.size() != 1)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 1");
    int code = *params[0]->asNumber();
    std::cout << "Program terminated with exit(" + std::to_string(code) + ")"
              << std::endl;
    std::exit(code);
}

ValuePtr Builtins::display(const std::vector<ValuePtr>& params) {
    for (auto& val : params) {
        if (auto str = std::dynamic_pointer_cast<StringValue>(val))
            std::cout << str->getStr();
        else
            std::cout << val->toString();
    }
    return std::make_shared<NilValue>();
}

ValuePtr Builtins::newline(const std::vector<ValuePtr>& params) {
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr Builtins::print(const std::vector<ValuePtr>& params) {
    for (auto& val : params) {
        std::cout << val->toString() << std::endl;
    }
    return std::make_shared<NilValue>();
}

void Builtins::initSymbolList() {
    EvalEnv::symbol_list["+"] = std::make_shared<BuiltinProcValue>(&add);
    EvalEnv::symbol_list["display"] =
        std::make_shared<BuiltinProcValue>(&display);
    EvalEnv::symbol_list["newline"] = std::make_shared<BuiltinProcValue>(&newline);
    EvalEnv::symbol_list["print"] =
        std::make_shared<BuiltinProcValue>(&print);
}
