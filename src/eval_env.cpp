#include "./eval_env.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>

#include "./builtins.h"
#include "./error.h"

std::unordered_map<std::string, ValuePtr> EvalEnv::symbol_list{};

std::vector<ValuePtr> EvalEnv::evalList(ValuePtr ls) {
    std::vector<ValuePtr> result;
    std::ranges::transform(ls->toVector(), std::back_inserter(result),
                           [this](ValuePtr v) { return this->eval(v); });
    return result;
}

ValuePtr EvalEnv::apply(ValuePtr proc, std::vector<ValuePtr> args) {
    if (auto func = dynamic_pointer_cast<BuiltinProcValue>(proc)) {
        return func->getFunc()(args);
    } else {
        throw LispError("Unimplemented.");
    }
}

ValuePtr EvalEnv::eval(ValuePtr expr) {
    using namespace std::literals;

    Builtins::initSymbolList();

    if (Value::isSelfEvaluating(expr))
        return expr;

    else if (Value::isNil(expr))
        throw LispError("Evaluating nil is prohibited.");

    else if (Value::isList(expr)) {
        auto ls = dynamic_pointer_cast<PairValue>(expr);
        std::vector<ValuePtr> vec = ls->toVector();

        if (vec[0]->asSymbol() == "define"s) {
            if (auto name = vec[1]->asSymbol()) {
                // TODO：抛出操作数异常
                symbol_list[*name] = vec[2];
                return std::make_shared<NilValue>();
            } else {
                throw LispError("Malformed define.");
            }
        } else {
            ValuePtr proc = eval(ls->car());
            std::vector<ValuePtr> args = evalList(ls->cdr());
            return apply(proc, args);
        }
    }

    else if (auto name = expr->asSymbol()) {
        if (auto value = symbol_list[*name])
            return value;
        else
            throw LispError("Variable " + *name + " not defined.");
    }

    else
        throw LispError("Unknown expression.");
}