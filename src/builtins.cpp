#include "./builtins.h"

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

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

ValuePtr Builtins::subtract(const std::vector<ValuePtr>& params) {
    if (params.size() > 2)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 2");

    if (params.size() == 1) {
        double minuend = 0;
        auto subtrahend = params[0]->asNumber();
        if (subtrahend == std::nullopt)
            throw LispError("Cannot divide a non-numeric value.");
        return std::make_shared<NumericValue>(minuend - *subtrahend);
    } else {
        auto minuend = params[0]->asNumber();
        auto subtrahend = params[1]->asNumber();
        if (subtrahend == std::nullopt || minuend == std::nullopt) {
            throw LispError("Cannot do subtraction on non-numeric values.");
        }
        return std::make_shared<NumericValue>(*minuend - *subtrahend);
    }
}

ValuePtr Builtins::multiply(const std::vector<ValuePtr>& params) {
    double total{1};
    for (auto& i : params) {
        auto num = i->asNumber();
        if (num == std::nullopt)
            throw LispError("Cannot multiply a non-numeric value.");
        total *= *num;
    }
    return std::make_shared<NumericValue>(total);
}

ValuePtr Builtins::divide(const std::vector<ValuePtr>& params) {
    if (params.size() > 2)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 2");

    if (params.size() == 1) {
        double dividend = 1;
        auto divisor = params[0]->asNumber();
        if (divisor == std::nullopt)
            throw LispError("Cannot divide a non-numeric value.");
        if (*divisor == 0) throw LispError("Cannot divide zero.");
        return std::make_shared<NumericValue>(dividend / *divisor);
    } else {
        auto dividend = params[0]->asNumber();
        auto divisor = params[1]->asNumber();
        if (divisor == std::nullopt || dividend == std::nullopt) {
            throw LispError("Cannot do division on non-numeric values.");
        }
        if (*divisor == 0) throw LispError("Cannot divide zero.");
        return std::make_shared<NumericValue>(*dividend / *divisor);
    }
}

ValuePtr Builtins::abs(const std::vector<ValuePtr>& params) {
    if (params.size() != 1)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 1");
    if (std::dynamic_pointer_cast<BooleanValue>(
            greaterOrEqual({params[0], std::make_shared<NumericValue>(0)}))
            ->getBool() == true)
        return params[0];
    return subtract(params);
}

// pair and list

ValuePtr Builtins::car(const std::vector<ValuePtr>& params) {
    if (params.size() != 1)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 1");
    if (auto pr = std::dynamic_pointer_cast<PairValue>(params[0])) {
        return pr->car();
    } else throw LispError("car: argument is not a pair");
}

ValuePtr Builtins::cdr(const std::vector<ValuePtr>& params) {
    if (params.size() != 1)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 1");
    if (auto pr = std::dynamic_pointer_cast<PairValue>(params[0])) {
        return pr->cdr();
    } else
        throw LispError("cdr: argument is not a pair");
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

// comp

ValuePtr Builtins::greater(const std::vector<ValuePtr>& params) {
    if (params.size() > 2)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 2");
    if (params.size() < 2)
        throw LispError("Too few arguments: " + std::to_string(params.size()) +
                        " < 2");

    auto num0 = params[0]->asNumber();
    auto num1 = params[1]->asNumber();
    if (num0 == std::nullopt || num1 == std::nullopt) {
        throw LispError("Cannot compare non-numeric values.");
    }
    return std::make_shared<BooleanValue>(*num0 > *num1);
}

ValuePtr Builtins::lesser(const std::vector<ValuePtr>& params) {
    if (params.size() > 2)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 2");
    if (params.size() < 2)
        throw LispError("Too few arguments: " + std::to_string(params.size()) +
                        " < 2");

    auto num0 = params[0]->asNumber();
    auto num1 = params[1]->asNumber();
    if (num0 == std::nullopt || num1 == std::nullopt) {
        throw LispError("Cannot compare non-numeric values.");
    }
    return std::make_shared<BooleanValue>(*num0 < *num1);
}

ValuePtr Builtins::equalNum(const std::vector<ValuePtr>& params) {
    if (params.size() > 2)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 2");
    if (params.size() < 2)
        throw LispError("Too few arguments: " + std::to_string(params.size()) +
                        " < 2");

    auto num0 = params[0]->asNumber();
    auto num1 = params[1]->asNumber();
    if (num0 == std::nullopt || num1 == std::nullopt) {
        throw LispError("Cannot compare non-numeric values.");
    }
    return std::make_shared<BooleanValue>(*num0 == *num1);
}

ValuePtr Builtins::greaterOrEqual(const std::vector<ValuePtr>& params) {
    if (std::dynamic_pointer_cast<BooleanValue>(greater(params))->getBool() ==
            true ||
        std::dynamic_pointer_cast<BooleanValue>(equalNum(params))->getBool() ==
            true)
        return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::lesserOrEqual(const std::vector<ValuePtr>& params) {
    if (std::dynamic_pointer_cast<BooleanValue>(lesser(params))->getBool() ==
            true ||
        std::dynamic_pointer_cast<BooleanValue>(equalNum(params))->getBool() ==
            true)
        return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isZero(const std::vector<ValuePtr>& params) {
    if (params.size() > 1)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > 1");
    return equalNum({params[0], std::make_shared<NumericValue>(0)});
}

void Builtins::initSymbolList() {
    // calc
    EvalEnv::symbol_list["+"] = std::make_shared<BuiltinProcValue>(&add);
    EvalEnv::symbol_list["-"] = std::make_shared<BuiltinProcValue>(&subtract);
    EvalEnv::symbol_list["*"] = std::make_shared<BuiltinProcValue>(&multiply);
    EvalEnv::symbol_list["/"] = std::make_shared<BuiltinProcValue>(&divide);
    EvalEnv::symbol_list["abs"] = std::make_shared<BuiltinProcValue>(&abs);

    // pair and list
    EvalEnv::symbol_list["car"] = std::make_shared<BuiltinProcValue>(&car);
    EvalEnv::symbol_list["cdr"] = std::make_shared<BuiltinProcValue>(&cdr);

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

    // core
    EvalEnv::symbol_list["display"] =
        std::make_shared<BuiltinProcValue>(&display);
    EvalEnv::symbol_list["newline"] =
        std::make_shared<BuiltinProcValue>(&newline);
    EvalEnv::symbol_list["print"] = std::make_shared<BuiltinProcValue>(&print);

    // comp
    EvalEnv::symbol_list["="] = std::make_shared<BuiltinProcValue>(&equalNum);
    EvalEnv::symbol_list[">"] = std::make_shared<BuiltinProcValue>(&greater);
    EvalEnv::symbol_list["<"] = std::make_shared<BuiltinProcValue>(&lesser);
    EvalEnv::symbol_list[">="] =
        std::make_shared<BuiltinProcValue>(&greaterOrEqual);
    EvalEnv::symbol_list["<="] =
        std::make_shared<BuiltinProcValue>(&lesserOrEqual);
    EvalEnv::symbol_list["zero?"] = std::make_shared<BuiltinProcValue>(&isZero);
}
