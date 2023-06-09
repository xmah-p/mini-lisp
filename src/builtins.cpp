#include "./builtins.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <ranges>

#include "./error.h"
#include "./eval_env.h"


namespace ranges = std::ranges;

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
        double num = val->asNumber();
        nums.push_back(num);
    }
    return nums;
}

// calc

ValuePtr Builtins::add(const std::vector<ValuePtr>& params, EvalEnv& env) {
    double total{0};
    auto nums = numericalize(params);
    for (auto num : nums) {
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
    for (auto num : nums) {
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
    checkArgNum(params, 1, 1);

    double num = params[0]->asNumber();
    return std::make_shared<NumericValue>(std::abs(num));
}

ValuePtr Builtins::expt(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 2);

    auto nums = numericalize(params);
    return std::make_shared<NumericValue>(std::pow(nums[0], nums[1]));
}

ValuePtr Builtins::quotient(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 2);

    auto nums = numericalize(params);
    return std::make_shared<NumericValue>(std::trunc(nums[0] / nums[1]));
}

ValuePtr Builtins::remainder(const std::vector<ValuePtr>& params,
                             EvalEnv& env) {
    checkArgNum(params, 2, 2);

    auto nums = numericalize(params);
    double q = std::trunc(nums[0] / nums[1]);
    return std::make_shared<NumericValue>(nums[0] - nums[1] * q);
}

ValuePtr Builtins::modulo(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 2);

    auto nums = numericalize(params);
    double q = std::trunc(nums[0] / nums[1]);
    q = q < 0 ? q - 1 : q;
    return std::make_shared<NumericValue>(nums[0] - nums[1] * q);
}

// pair and list

ValuePtr Builtins::car(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 1);

    if (auto pr = std::dynamic_pointer_cast<PairValue>(params[0]))
        return pr->car();
    else
        throw TypeError(params[0]->toString() + " is not a pair");
}

ValuePtr Builtins::cdr(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 1);

    if (auto pr = std::dynamic_pointer_cast<PairValue>(params[0]))
        return pr->cdr();
    else
        throw TypeError(params[0]->toString() + " is not a pair");
}

ValuePtr Builtins::cons(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 2);

    return std::make_shared<PairValue>(params[0], params[1]);
}

ValuePtr Builtins::length(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 1);

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
        ranges::copy(vec, std::back_inserter(appended));
    }
    return Value::makeList(appended);
}

ValuePtr Builtins::map(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 2);

    std::vector<ValuePtr> mapped;
    if (!Value::isProcedure(params[0]))
        throw TypeError(params[0]->toString() + " is not a procedure");
    auto list = vectorize(params[1]);

    ranges::transform(
        list.begin(), list.end(), std::back_inserter(mapped),
        [&](ValuePtr arg) { return env.apply(params[0], {arg}); });
    return Value::makeList(mapped);
}

ValuePtr Builtins::filter(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 2);

    std::vector<ValuePtr> filtered;
    if (!Value::isProcedure(params[0]))
        throw TypeError(params[0]->toString() + " is not a procedure");
    auto list = vectorize(params[1]);

    ranges::copy_if(list.begin(), list.end(), std::back_inserter(filtered),
                    [&](ValuePtr val) {
                        return !Value::isVirtual(env.apply(params[0], {val}));
                    });
    return Value::makeList(filtered);
}

ValuePtr Builtins::reduce(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 2);

    std::vector<ValuePtr> reduced;
    if (!Value::isProcedure(params[0]))
        throw TypeError(params[0]->toString() + " is not a procedure");
    auto list = vectorize(params[1]);

    return std::accumulate(list.begin() + 1, list.end(), list.front(),
                           [&](ValuePtr arg0, ValuePtr arg1) {
                               return env.apply(params[0], {arg0, arg1});
                           });
}

// type

ValuePtr Builtins::isAtom(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 1);

    return std::make_shared<BooleanValue>(!Value::isPair(params[0]) &&
                                          !Value::isProcedure(params[0]));
}

ValuePtr Builtins::isBoolean(const std::vector<ValuePtr>& params,
                             EvalEnv& env) {
    checkArgNum(params, 1, 1);

    return std::make_shared<BooleanValue>(Value::isBoolean(params[0]));
}

ValuePtr Builtins::isInteger(const std::vector<ValuePtr>& params,
                             EvalEnv& env) {
    checkArgNum(params, 1, 1);

    if (Value::isNumeric(params[0]))
        return std::make_shared<BooleanValue>(
            fmod(params[0]->asNumber(), 1.0) == 0.0);

    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isList(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 1);

    return std::make_shared<BooleanValue>(Value::isList(params[0]));
}

ValuePtr Builtins::isNumber(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 1);

    return std::make_shared<BooleanValue>(Value::isNumeric(params[0]));
}

ValuePtr Builtins::isNull(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 1);

    return std::make_shared<BooleanValue>(Value::isNil(params[0]));
}

ValuePtr Builtins::isPair(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 1);

    return std::make_shared<BooleanValue>(Value::isPair(params[0]));
}

ValuePtr Builtins::isProcedure(const std::vector<ValuePtr>& params,
                               EvalEnv& env) {
    checkArgNum(params, 1, 1);

    return std::make_shared<BooleanValue>(Value::isProcedure(params[0]));
}

ValuePtr Builtins::isString(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 1);

    return std::make_shared<BooleanValue>(Value::isString(params[0]));
}

ValuePtr Builtins::isSymbol(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    return std::make_shared<BooleanValue>(Value::isSymbol(params[0]));
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
    if (!params.empty()) code = static_cast<int>(params[0]->asNumber());

    std::cout << "Program terminated with exit(" + std::to_string(code) + ")"
              << std::endl;
    std::exit(code);
}

ValuePtr Builtins::display(const std::vector<ValuePtr>& params, EvalEnv& env) {
    for (auto& val : params) {
        if (auto str = std::dynamic_pointer_cast<StringValue>(val))
            std::cout << str->getVal();
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
    checkArgNum(params, 2, 2);

    if ((Value::isNumeric(params[0]) || Value::isSymbol(params[0]) ||
         Value::isBoolean(params[0]) || Value::isNil(params[0])) &&
        params[0]->toString() == params[1]->toString())
        return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(params[0] == params[1]);
}

ValuePtr Builtins::isEqualValue(const std::vector<ValuePtr>& params,
                                EvalEnv& env) {
    checkArgNum(params, 2, 2);

    return std::make_shared<BooleanValue>(params[0]->toString() ==
                                          params[1]->toString());
}

ValuePtr Builtins::isNot(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 1);

    return std::make_shared<BooleanValue>(Value::isVirtual(params[0]));
}

ValuePtr Builtins::greater(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 2);

    auto nums = numericalize(params);
    return std::make_shared<BooleanValue>(nums[0] > nums[1]);
}

ValuePtr Builtins::lesser(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 2);

    auto nums = numericalize(params);
    return std::make_shared<BooleanValue>(nums[0] < nums[1]);
}

ValuePtr Builtins::equalNum(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 2);

    auto nums = numericalize(params);
    return std::make_shared<BooleanValue>(nums[0] == nums[1]);
}

ValuePtr Builtins::greaterOrEqual(const std::vector<ValuePtr>& params,
                                  EvalEnv& env) {
    checkArgNum(params, 2, 2);

    auto nums = numericalize(params);
    return std::make_shared<BooleanValue>(nums[0] >= nums[1]);
}

ValuePtr Builtins::lesserOrEqual(const std::vector<ValuePtr>& params,
                                 EvalEnv& env) {
    checkArgNum(params, 2, 2);

    auto nums = numericalize(params);
    return std::make_shared<BooleanValue>(nums[0] <= nums[1]);
}

ValuePtr Builtins::isZero(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 1);

    if (Value::isNumeric(params[0]))
        return std::make_shared<BooleanValue>(params[0]->asNumber() == 0.0);
    return std::make_shared<BooleanValue>(false);
}

ValuePtr Builtins::isEven(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 1);

    double num = params[0]->asNumber();
    return std::make_shared<BooleanValue>(std::fmod(num, 2) == 0.0);
}

ValuePtr Builtins::isOdd(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 1);

    double num = params[0]->asNumber();
    return std::make_shared<BooleanValue>(std::fmod(num, 2) != 0.0 &&
                                          std::fmod(num, 1) == 0.0);
}

ValuePtr Builtins::max(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    auto nums = numericalize(params);
    double res = *ranges::max_element(nums);
    return std::make_shared<NumericValue>(res);
}

ValuePtr Builtins::min(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1);

    auto nums = numericalize(params);
    double res = *ranges::min_element(nums);
    return std::make_shared<NumericValue>(res);
}

ValuePtr Builtins::listRef(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 2);

    auto vec = vectorize(params[0]);
    std::size_t idx = params[1]->asNumber();
    if (idx >= vec.size())
        throw LispError("List index out of range: " + params[0]->toString() +
                        "[" + std::to_string(idx) + "]");
    return vec[idx];
}

ValuePtr Builtins::listTail(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 2);

    auto vec = vectorize(params[0]);
    std::size_t idx = params[1]->asNumber();
    if (idx >= vec.size())
        throw LispError("List index out of range: " + params[0]->toString() +
                        "[" + std::to_string(idx) + "]");
    return Value::makeList({vec.begin() + idx, vec.end()});
}

ValuePtr Builtins::forEach(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 2);

    if (!Value::isProcedure(params[0]))
        throw TypeError(params[0]->toString() + " is not a procedure");
    auto list = vectorize(params[1]);

    ranges::for_each(list.begin(), list.end(),
                     [&](ValuePtr arg) { return env.apply(params[0], {arg}); });
    return std::make_shared<NilValue>();
}

ValuePtr Builtins::listReverse(const std::vector<ValuePtr>& params,
                               EvalEnv& env) {
    checkArgNum(params, 1, 1);

    auto ls = vectorize(params[0]);
    decltype(ls) reversed;
    std::reverse_copy(ls.begin(), ls.end(), std::back_inserter(reversed));
    return Value::makeList(reversed);
}

ValuePtr Builtins::member(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 2);

    auto ls = vectorize(params[1]);
    auto it = ls.begin();
    while (it != ls.end() && (*it)->toString() != params[0]->toString()) ++it;
    if (it == ls.end()) return std::make_shared<BooleanValue>(false);
    return Value::makeList({it, ls.end()});
}

ValuePtr Builtins::numberToString(const std::vector<ValuePtr>& params,
                                  EvalEnv& env) {
    checkArgNum(params, 1);

    double num = params[0]->asNumber();
    std::string str;
    if (std::fmod(num, 1.0) == 0.0)
        str = std::to_string(static_cast<int>(num));
    else
        str = std::to_string(num);
    return std::make_shared<StringValue>(str);
}

ValuePtr Builtins::stringToNumber(const std::vector<ValuePtr>& params,
                                  EvalEnv& env) {
    checkArgNum(params, 1);

    std::string str = params[0]->asString();
    try {
        double num = std::stod(str);
        return std::make_shared<NumericValue>(num);
    } catch (std::invalid_argument&) {
        throw LispError("Invalid argument: " + str);
    }
}

ValuePtr Builtins::makeStr(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 2);

    double n = params[0]->asNumber();
    if (n < 0)
        throw LispError("Cannot make string with negative number " +
                        params[0]->toString());
    char c = ' ';
    if (params.size() == 2) {
        std::string tmp = params[1]->asString();
        if (tmp.length() != 1)
            throw TypeError("\"" + tmp + "\"" + " is not a char");
        c = tmp[0];
    }
    return std::make_shared<StringValue>(
        std::string(static_cast<std::size_t>(n), c));
}

ValuePtr Builtins::strRef(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 2);

    std::string str = params[0]->asString();
    std::size_t n = params[1]->asNumber();
    if (n >= str.length() || n < 0)
        throw LispError("Index " + params[1]->toString() +
                        " is out of bound of \"" + str + "\"");

    return std::make_shared<StringValue>(std::string(1, str[n]));
}

ValuePtr Builtins::strLength(const std::vector<ValuePtr>& params,
                             EvalEnv& env) {
    checkArgNum(params, 1, 1);

    std::string str = params[0]->asString();
    return std::make_shared<NumericValue>(str.length());
}

ValuePtr Builtins::subStr(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 2, 3);

    std::string str = params[0]->asString();
    std::size_t pos = params[1]->asNumber();
    std::size_t n = std::string::npos;
    if (params.size() == 3) n = params[2]->asNumber();

    if (n < 0 || pos < 0 || pos >= str.length())
        throw LispError("Range {pos=" + params[1]->toString() +
                        ", n=" + params[2]->toString() + "} out of bound");
    return std::make_shared<StringValue>(str.substr(pos, n));
}

ValuePtr Builtins::strAppend(const std::vector<ValuePtr>& params,
                             EvalEnv& env) {
    checkArgNum(params, 2, 2);

    std::string str0 = params[0]->asString();
    std::string str1 = params[1]->asString();
    return std::make_shared<StringValue>(str0 + str1);
}

ValuePtr Builtins::strCopy(const std::vector<ValuePtr>& params, EvalEnv& env) {
    checkArgNum(params, 1, 1);

    std::string str = params[0]->asString();
    return std::make_shared<StringValue>(str);
}

extern const std::unordered_map<std::string, BuiltinFuncType*>
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
                               {"even?", isEven},  // comp
                               {"max", max},
                               {"min", min},
                               {"list-ref", listRef},
                               {"list-tail", listTail},
                               {"for-each", forEach},
                               {"reverse", listReverse},
                               {"member", member},
                               {"number->string", numberToString},
                               {"string->number", stringToNumber},
                               {"make-string", makeStr},
                               {"string-ref", strRef},
                               {"string-length", strLength},
                               {"string-append", strAppend},
                               {"string-copy", strCopy},
                               {"substring", subStr}
                               };