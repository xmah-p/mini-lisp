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

EvalEnv::EvalEnv() {
    using namespace Builtins;

    // calc
    symbol_list["+"] = std::make_shared<BuiltinProcValue>(&add);
    symbol_list["-"] = std::make_shared<BuiltinProcValue>(&subtract);
    symbol_list["*"] = std::make_shared<BuiltinProcValue>(&multiply);
    symbol_list["/"] = std::make_shared<BuiltinProcValue>(&divide);
    symbol_list["abs"] = std::make_shared<BuiltinProcValue>(&Builtins::abs);

    // pair and list
    symbol_list["car"] = std::make_shared<BuiltinProcValue>(&car);
    symbol_list["cdr"] = std::make_shared<BuiltinProcValue>(&cdr);

    // type
    symbol_list["atom?"] = std::make_shared<BuiltinProcValue>(&isAtom);
    symbol_list["boolean?"] = std::make_shared<BuiltinProcValue>(&isBoolean);
    symbol_list["integer?"] = std::make_shared<BuiltinProcValue>(&isInteger);
    symbol_list["list?"] = std::make_shared<BuiltinProcValue>(&isList);
    symbol_list["number?"] = std::make_shared<BuiltinProcValue>(&isNumber);
    symbol_list["null?"] = std::make_shared<BuiltinProcValue>(&isNull);
    symbol_list["pair?"] = std::make_shared<BuiltinProcValue>(&isPair);
    symbol_list["procedure?"] =
        std::make_shared<BuiltinProcValue>(&isProcedure);
    symbol_list["string?"] = std::make_shared<BuiltinProcValue>(&isString);
    symbol_list["symbol?"] = std::make_shared<BuiltinProcValue>(&isSymbol);

    // core
    symbol_list["display"] = std::make_shared<BuiltinProcValue>(&display);
    symbol_list["newline"] = std::make_shared<BuiltinProcValue>(&newline);
    symbol_list["print"] = std::make_shared<BuiltinProcValue>(&print);
    symbol_list["exit"] = std::make_shared<BuiltinProcValue>(&Builtins::exit);

    // comp
    symbol_list["="] = std::make_shared<BuiltinProcValue>(&equalNum);
    symbol_list[">"] = std::make_shared<BuiltinProcValue>(&greater);
    symbol_list["<"] = std::make_shared<BuiltinProcValue>(&lesser);
    symbol_list[">="] = std::make_shared<BuiltinProcValue>(&greaterOrEqual);
    symbol_list["<="] = std::make_shared<BuiltinProcValue>(&lesserOrEqual);
    symbol_list["zero?"] = std::make_shared<BuiltinProcValue>(&isZero);
}

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