#include "./eval_env.h"

#include <iostream>
#include <unordered_map>

#include "./error.h"

std::unordered_map<std::string, ValuePtr> EvalEnv::symbol_list{};

ValuePtr EvalEnv::eval(ValuePtr expr) {
    using namespace std::literals;

    if (Value::isSelfEvaluating(expr))
        return expr;

    else if (Value::isNil(expr))
        throw LispError("Evaluating nil is prohibited.");

    else if (Value::isList(expr)) {
        std::vector<ValuePtr> vec = expr->toVector();

        if (vec[0]->asSymbol() == "define"s) {
            if (auto name = vec[1]->asSymbol()) {
                symbol_list[*name] = vec[2];
                return std::make_shared<NilValue>();
            } else {
                throw LispError("Malformed define.");
            }
        }

        else throw LispError("Unimplemented.");
    }

    else if (auto name = expr->asSymbol()) {
        if (auto value = symbol_list[*name])
            return value;
        else
            throw LispError("Variable " + *name + " not defined.");
    }

    else throw LispError("Unknown expression.");
}