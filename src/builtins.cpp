#include "./builtins.h"

#include <iostream>
#include <memory>
#include <string>

#include "./error.h"
#include "./eval_env.h"

// calc

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

// type

ValuePtr Builtins::isAtom(const std::vector<ValuePtr>& params) {
    if (params.size() != 1)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 1");
    if (Value::isNil(params[0]) || Value::isSelfEvaluating(params[0]) ||
        params[0]->asSymbol())
        return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isBoolean(const std::vector<ValuePtr>& params) {
    if (params.size() != 1)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 1");
    if (std::dynamic_pointer_cast<BooleanValue>(params[0]))
        return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isInteger(const std::vector<ValuePtr>& params) {
    if (params.size() != 1)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 1");
    if (double num = *params[0]->asNumber()) {
        return (num - static_cast<int>(num) == 0)
                   ? std::make_shared<BooleanValue>(true)
                   : std::make_shared<BooleanValue>(false);
    }
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isList(const std::vector<ValuePtr>& params) {
    if (params.size() != 1)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 1");
    if (Value::isList(params[0])) return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isNumber(const std::vector<ValuePtr>& params) {
    if (params.size() != 1)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 1");
    if (params[0]->asNumber()) return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isNull(const std::vector<ValuePtr>& params) {
    if (params.size() != 1)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 1");
    if (Value::isNil(params[0])) return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isPair(const std::vector<ValuePtr>& params) {
    if (params.size() != 1)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 1");
    if (Value::isPair(params[0])) return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isProcedure(const std::vector<ValuePtr>& params) {
    if (params.size() != 1)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 1");
    if (Value::isProcedure(params[0]))
        return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isString(const std::vector<ValuePtr>& params) {
    if (params.size() != 1)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 1");
    if (std::dynamic_pointer_cast<StringValue>(params[0]))
        return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isSymbol(const std::vector<ValuePtr>& params) {
    if (params.size() != 1)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 1");
    if (params[0]->asSymbol()) return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

// core

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
    // calc
    EvalEnv::symbol_list["+"] = std::make_shared<BuiltinProcValue>(&add);

    // core
    EvalEnv::symbol_list["display"] =
        std::make_shared<BuiltinProcValue>(&display);
    EvalEnv::symbol_list["newline"] =
        std::make_shared<BuiltinProcValue>(&newline);
    EvalEnv::symbol_list["print"] = std::make_shared<BuiltinProcValue>(&print);

    // type
    EvalEnv::symbol_list["atom?"] = std::make_shared<BuiltinProcValue>(&isAtom);
    EvalEnv::symbol_list["boolean?"] =
        std::make_shared<BuiltinProcValue>(&isBoolean);
    EvalEnv::symbol_list["integer?"] =
        std::make_shared<BuiltinProcValue>(&isInteger);
    EvalEnv::symbol_list["list?"] = std::make_shared<BuiltinProcValue>(&isList);
    EvalEnv::symbol_list["number?"] =
        std::make_shared<BuiltinProcValue>(&isNumber);
    EvalEnv::symbol_list["null?"] = std::make_shared<BuiltinProcValue>(&isNull);
    EvalEnv::symbol_list["pair?"] = std::make_shared<BuiltinProcValue>(&isPair);
    EvalEnv::symbol_list["procedure?"] =
        std::make_shared<BuiltinProcValue>(&isProcedure);
    EvalEnv::symbol_list["string?"] =
        std::make_shared<BuiltinProcValue>(&isString);
    EvalEnv::symbol_list["symbol?"] =
        std::make_shared<BuiltinProcValue>(&isSymbol);
}
