#include "./eval_env.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "./builtins.h"
#include "./error.h"
#include "./forms.h"

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

        if (auto name = vec[0]->asSymbol()) {
            
            if (SpecialForm::form_list.find(*name) !=
                SpecialForm::form_list.end()) {
                auto form = SpecialForm::form_list.at(*name);
                return form(ls->cdr()->toVector(), *this);
            }

            else if (auto proc = symbol_list[*name]) {
                std::vector<ValuePtr> args;
                if (Value::isList(ls->cdr()))
                    args = evalList(ls->cdr());
                else
                    args.push_back(eval(ls->cdr()));
                return apply(proc, args);
            }

            else
                throw LispError("Unbound variable " + *name);
        } 
        
        else
            throw LispError("Not a procedure: " + vec[0]->toString());
    }

    else if (auto name = expr->asSymbol()) {
        
        if (auto value = symbol_list[*name])
            return value;
        else
            throw LispError("Variable " + *name + " not defined.");
    }

    else
        throw LispError("Unknown expression.");

    return std::make_shared<NilValue>();
}