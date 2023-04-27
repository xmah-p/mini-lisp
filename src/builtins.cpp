#include "./builtins.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

#include "./error.h"
#include "./eval_env.h"

// helper functions
void Builtins::checkArgNum(const std::vector<ValuePtr>& params, std::size_t min,
                           std::size_t max) {
    if (params.size() > max)
        throw LispError("Too many arguments: " + std::to_string(params.size()) +
                        " > " + std::to_string(max));
    if (params.size() < min)
        throw LispError("Too few arguments: " + std::to_string(params.size()) +
                        " < " + std::to_string(min));
}

std::vector<ValuePtr> Builtins::vectorize(const ValuePtr& ls) {
    if (!Value::isList(ls))
        throw LispError("Malformed list: " + ls->toString());
    return ls->toVector();
}

std::vector<double> Builtins::numericalize(const std::vector<ValuePtr>& vals) {
    std::vector<double> nums;
    for (auto& val : vals) {
        auto num = val->asNumber();
        if (num == std::nullopt)
            throw LispError("Expected numeric value, got " + val->toString());
        nums.push_back(*num);
    }
    return nums;
}

// calc

ValuePtr Builtins::add(const std::vector<ValuePtr>& params, EvalEnv& env) {
    double total{0};
    auto nums = numericalize(params);
    for (auto& num : nums) {
        total += num;
    }
    return std::make_shared<NumericValue>(total);
}

ValuePtr Builtins::subtract(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 2);

    double minuend = 0;
    double subtrahend = 0;
    auto nums = numericalize(params);
    if (nums.size() == 1)
        subtrahend = nums[0];
    else {
        minuend = nums[0];
        subtrahend = nums[1];
    }
    return std::make_shared<NumericValue>(minuend - subtrahend);
}

ValuePtr Builtins::multiply(const std::vector<ValuePtr>& params, EvalEnv& env) {
    double total{1};
    auto nums = numericalize(params);
    for (auto& num : nums) {
        total *= num;
    }
    return std::make_shared<NumericValue>(total);
}

ValuePtr Builtins::divide(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 2);

    double dividend = 1;
    double divisor = 1;
    auto nums = numericalize(params);
    if (nums.size() == 1)
        divisor = nums[0];
    else {
        dividend = nums[0];
        divisor = nums[1];
    }
    return std::make_shared<NumericValue>(dividend / divisor);
}

ValuePtr Builtins::abs(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    double num = numericalize({params[0]})[0];
    return std::make_shared<NumericValue>(std::abs(num));
}

ValuePtr Builtins::expt(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2);

    auto nums = numericalize({params[0], params[1]});
    return std::make_shared<NumericValue>(std::pow(nums[0], nums[1]));
}

ValuePtr Builtins::quotient(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2);

    auto nums = numericalize({params[0], params[1]});
    return std::make_shared<NumericValue>(std::trunc(nums[0] / nums[1]));
}

ValuePtr Builtins::remainder(const std::vector<ValuePtr>& params,
                             EvalEnv& env) {
    checkArgNum(params, 2);

    auto nums = numericalize({params[0], params[1]});
    double q = *quotient(params, env)->asNumber();
    return std::make_shared<NumericValue>(nums[0] - nums[1] * q);
}

ValuePtr Builtins::modulo(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2);

    auto nums = numericalize({params[0], params[1]});
    double q = *quotient(params, env)->asNumber();
    q = q < 0 ? q - 1 : q;
    return std::make_shared<NumericValue>(nums[0] - nums[1] * q);
}

// pair and list

ValuePtr Builtins::car(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    if (auto pr = std::dynamic_pointer_cast<PairValue>(params[0]))
        return pr->car();
    else
        throw LispError("car: argument is not a pair");
}

ValuePtr Builtins::cdr(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    if (auto pr = std::dynamic_pointer_cast<PairValue>(params[0]))
        return pr->cdr();
    else
        throw LispError("cdr: argument is not a pair");
}

ValuePtr Builtins::cons(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2);

    return std::make_shared<PairValue>(params[0], params[1]);
}

ValuePtr Builtins::length(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    auto vec = vectorize(params[0]);
    return std::make_shared<NumericValue>(vec.size());
}

ValuePtr Builtins::list(const std::vector<ValuePtr>& params, EvalEnv& env) {
    return Value::makeList(params);
}

ValuePtr Builtins::append(const std::vector<ValuePtr>& params, EvalEnv& env) {
    std::vector<ValuePtr> appended;
    for (auto& arg : params) {
        auto vec = vectorize(arg);
        std::ranges::copy(vec, std::back_inserter(appended));
    }
    return Value::makeList(appended);
}

ValuePtr Builtins::map(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 2);

    std::vector<ValuePtr> mapped;
    if (!Value::isProcedure(params[0]))
        throw LispError("Not a procedure: " + params[0]->toString());

    auto list = vectorize(params[1]);
    std::ranges::transform(
        list.begin(), list.end(), std::back_inserter(mapped),
        [&](ValuePtr arg) { return env.apply(params[0], {arg}); });
    return Value::makeList(mapped);
}

ValuePtr Builtins::filter(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2);

    std::vector<ValuePtr> filtered;
    if (!Value::isProcedure(params[0]))
        throw LispError("Not a procedure: " + params[0]->toString());
    auto list = vectorize(params[1]);
    std::ranges::copy_if(
        list.begin(), list.end(), std::back_inserter(filtered),
        [&](ValuePtr val) {
            if (auto cond = std::dynamic_pointer_cast<BooleanValue>(val))
                if (!cond->getBool()) return true;
            return false;
        });
    return Value::makeList(filtered);
}

ValuePtr Builtins::reduce(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2);

    std::vector<ValuePtr> reduced;
    if (!Value::isProcedure(params[0]))
        throw LispError("Not a procedure: " + params[0]->toString());
    auto list = vectorize(params[1]);
    return std::accumulate(list.begin() + 1, list.end(), list.front(),
                           [&](ValuePtr arg0, ValuePtr arg1) {
                               return env.apply(params[0], {arg0, arg1});
                           });
}

// type

ValuePtr Builtins::isAtom(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    if (Value::isNil(params[0]) || Value::isSelfEvaluating(params[0]) ||
        params[0]->asSymbol())
        return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isBoolean(const std::vector<ValuePtr>& params,
                             EvalEnv& env) {
    checkArgNum(params, 1);

    if (std::dynamic_pointer_cast<BooleanValue>(params[0]))
        return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isInteger(const std::vector<ValuePtr>& params,
                             EvalEnv& env) {
    checkArgNum(params, 1);

    if (double num = *params[0]->asNumber()) {
        return (num - static_cast<int>(num) == 0)
                   ? std::make_shared<BooleanValue>(true)
                   : std::make_shared<BooleanValue>(false);
    }
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isList(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    if (Value::isList(params[0])) return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isNumber(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    if (params[0]->asNumber()) return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isNull(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    if (Value::isNil(params[0])) return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isPair(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    if (Value::isPair(params[0])) return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isProcedure(const std::vector<ValuePtr>& params,
                               EvalEnv& env) {
    checkArgNum(params, 1);

    if (Value::isProcedure(params[0]))
        return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isString(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    if (std::dynamic_pointer_cast<StringValue>(params[0]))
        return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isSymbol(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    if (params[0]->asSymbol()) return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

// core

ValuePtr Builtins::apply(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 2);

    auto args = vectorize(params[1]);
    return env.apply(params[0], args);
}

ValuePtr Builtins::eval(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 1);

    return env.eval(params[0]);
}

ValuePtr Builtins::exit(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 0, 1);

    int code = 0;
    auto num_vec = numericalize({params[0]});
    if (!num_vec.empty()) code = static_cast<int>(num_vec[0]);
    std::cout << "Program terminated with exit(" + std::to_string(code) + ")"
              << std::endl;
    std::exit(code);
}

ValuePtr Builtins::display(const std::vector<ValuePtr>& params, EvalEnv& env) {
    for (auto& val : params) {
        if (auto str = std::dynamic_pointer_cast<StringValue>(val))
            std::cout << str->getStr();
        else
            std::cout << val->toString();
    }
    return std::make_shared<NilValue>();
}

ValuePtr Builtins::newline(const std::vector<ValuePtr>& params, EvalEnv& env) {
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr Builtins::displayln(const std::vector<ValuePtr>& params,
                             EvalEnv& env) {
    display(params, env);
    return newline({}, env);
}

ValuePtr Builtins::print(const std::vector<ValuePtr>& params, EvalEnv& env) {
    for (auto& val : params) {
        std::cout << val->toString() << std::endl;
    }
    return std::make_shared<NilValue>();
}

ValuePtr Builtins::error(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) throw LispError("0");
    throw LispError(params[0]->toString());
}

// comp

ValuePtr Builtins::isEq(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2);

    if ((std::dynamic_pointer_cast<NumericValue>(params[0]) ||
         std::dynamic_pointer_cast<SymbolValue>(params[0]) ||
         std::dynamic_pointer_cast<BooleanValue>(params[0]) ||
         std::dynamic_pointer_cast<NilValue>(params[0])) &&
        params[0]->toString() == params[1]->toString())
        return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(params[0] == params[1]);
}

ValuePtr Builtins::isEqualValue(const std::vector<ValuePtr>& params,
                                EvalEnv& env) {
    checkArgNum(params, 2);

    return std::make_shared<BooleanValue>(params[0]->toString() ==
                                          params[1]->toString());
}

ValuePtr Builtins::isNot(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    if (auto cond = std::dynamic_pointer_cast<BooleanValue>(params[0]))
        if (!cond->getBool()) return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::greater(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2);

    auto num0 = params[0]->asNumber();
    auto num1 = params[1]->asNumber();
    if (num0 == std::nullopt || num1 == std::nullopt) {
        throw LispError("Cannot compare non-numeric values.");
    }
    return std::make_shared<BooleanValue>(*num0 > *num1);
}

ValuePtr Builtins::lesser(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2);

    auto num0 = params[0]->asNumber();
    auto num1 = params[1]->asNumber();
    if (num0 == std::nullopt || num1 == std::nullopt) {
        throw LispError("Cannot compare non-numeric values.");
    }
    return std::make_shared<BooleanValue>(*num0 < *num1);
}

ValuePtr Builtins::equalNum(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2);

    auto num0 = params[0]->asNumber();
    auto num1 = params[1]->asNumber();
    if (num0 == std::nullopt || num1 == std::nullopt) {
        throw LispError("Cannot compare non-numeric values.");
    }
    return std::make_shared<BooleanValue>(*num0 == *num1);
}

ValuePtr Builtins::greaterOrEqual(const std::vector<ValuePtr>& params,
                                  EvalEnv& env) {
    if (std::dynamic_pointer_cast<BooleanValue>(greater(params, env))
                ->getBool() == true ||
        std::dynamic_pointer_cast<BooleanValue>(equalNum(params, env))
                ->getBool() == true)
        return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::lesserOrEqual(const std::vector<ValuePtr>& params,
                                 EvalEnv& env) {
    if (std::dynamic_pointer_cast<BooleanValue>(lesser(params, env))
                ->getBool() == true ||
        std::dynamic_pointer_cast<BooleanValue>(equalNum(params, env))
                ->getBool() == true)
        return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isZero(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    return equalNum({params[0], std::make_shared<NumericValue>(0)}, env);
}

ValuePtr Builtins::isEven(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    if (auto num = params[0]->asNumber()) {
        return std::make_shared<BooleanValue>(std::fmod(*num, 2) == 0.0);
    }
    throw LispError(params[0]->toString() + " is not number");
}

ValuePtr Builtins::isOdd(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    if (auto num = params[0]->asNumber()) {
        return std::make_shared<BooleanValue>(std::fmod(*num, 2) != 0.0 &&
                                              std::fmod(*num, 1) == 0.0);
    }
    throw LispError(params[0]->toString() + " is not number");
}

const std::unordered_map<std::string, BuiltinFuncType*>
    Builtins::builtin_forms = {{"+", add},
                               {"-", subtract},
                               {"*", multiply},
                               {"/", divide},
                               {"abs", abs},
                               {"expt", expt},
                               {"quotient", quotient},
                               {"modulo", modulo},
                               {"remainder", remainder},  // calc
                               {"car", car},
                               {"cdr", cdr},
                               {"append", append},
                               {"cons", cons},
                               {"length", length},
                               {"list", list},
                               {"map", map},
                               {"filter", filter},
                               {"reduce", reduce},  // pair and list
                               {"atom?", isAtom},
                               {"boolean?", isBoolean},
                               {"integer?", isInteger},
                               {"list?", isList},
                               {"number?", isNumber},
                               {"null?", isNull},
                               {"pair?", isPair},
                               {"procedure?", isProcedure},
                               {"string?", isString},
                               {"symbol?", isSymbol},  // type
                               {"apply", apply},
                               {"eval", eval},
                               {"display", display},
                               {"newline", newline},
                               {"displayln", displayln},
                               {"print", print},
                               {"error", error},
                               {"exit", exit},  // core
                               {"eq?", isEq},
                               {"equal?", isEqualValue},
                               {"not", isNot},
                               {"=", equalNum},
                               {">", greater},
                               {"<", lesser},
                               {">=", greaterOrEqual},
                               {"<=", lesserOrEqual},
                               {"zero?", isZero},
                               {"odd?", isOdd},
                               {"even?", isEven}};