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

std::shared_ptr<EvalEnv> EvalEnv::createGlobal() {
    auto global = std::make_shared<EvalEnv>();

    using namespace Builtins;

    // calc
    global->symbol_list["+"] = std::make_shared<BuiltinProcValue>(&add);
    global->symbol_list["-"] = std::make_shared<BuiltinProcValue>(&subtract);
    global->symbol_list["*"] = std::make_shared<BuiltinProcValue>(&multiply);
    global->symbol_list["/"] = std::make_shared<BuiltinProcValue>(&divide);
    global->symbol_list["abs"] =
        std::make_shared<BuiltinProcValue>(&Builtins::abs);

    // pair and list
    global->symbol_list["car"] = std::make_shared<BuiltinProcValue>(&car);
    global->symbol_list["cdr"] = std::make_shared<BuiltinProcValue>(&cdr);

    // type
    global->symbol_list["atom?"] = std::make_shared<BuiltinProcValue>(&isAtom);
    global->symbol_list["boolean?"] =
        std::make_shared<BuiltinProcValue>(&isBoolean);
    global->symbol_list["integer?"] =
        std::make_shared<BuiltinProcValue>(&isInteger);
    global->symbol_list["list?"] = std::make_shared<BuiltinProcValue>(&isList);
    global->symbol_list["number?"] =
        std::make_shared<BuiltinProcValue>(&isNumber);
    global->symbol_list["null?"] = std::make_shared<BuiltinProcValue>(&isNull);
    global->symbol_list["pair?"] = std::make_shared<BuiltinProcValue>(&isPair);
    global->symbol_list["procedure?"] =
        std::make_shared<BuiltinProcValue>(&isProcedure);
    global->symbol_list["string?"] =
        std::make_shared<BuiltinProcValue>(&isString);
    global->symbol_list["symbol?"] =
        std::make_shared<BuiltinProcValue>(&isSymbol);

    // core
    global->symbol_list["display"] =
        std::make_shared<BuiltinProcValue>(&display);
    global->symbol_list["newline"] =
        std::make_shared<BuiltinProcValue>(&newline);
    global->symbol_list["print"] = std::make_shared<BuiltinProcValue>(&print);
    global->symbol_list["exit"] =
        std::make_shared<BuiltinProcValue>(&Builtins::exit);

    // comp
    global->symbol_list["="] = std::make_shared<BuiltinProcValue>(&equalNum);
    global->symbol_list[">"] = std::make_shared<BuiltinProcValue>(&greater);
    global->symbol_list["<"] = std::make_shared<BuiltinProcValue>(&lesser);
    global->symbol_list[">="] =
        std::make_shared<BuiltinProcValue>(&greaterOrEqual);
    global->symbol_list["<="] =
        std::make_shared<BuiltinProcValue>(&lesserOrEqual);
    global->symbol_list["zero?"] = std::make_shared<BuiltinProcValue>(&isZero);

    return global;
}

std::shared_ptr<EvalEnv> EvalEnv::createChild(
    const std::vector<std::string>& params, const std::vector<ValuePtr>& args) {
    auto child = std::make_shared<EvalEnv>(this->shared_from_this());
    if (params.size() != args.size())
        throw LispError("Procedure expected " + std::to_string(params.size()) +
                        " parameters, got " + std::to_string(args.size()));
    for (unsigned int i = 0; i < params.size(); ++i) {
        child->symbol_list[params[i]] = args[i];
    }
    return child;
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
    } else if (auto func = std::dynamic_pointer_cast<LambdaValue>(proc)) {
        return func->apply(args);
    } else throw LispError("Fail to apply");
}

ValuePtr EvalEnv::lookupBinding(std::string name) const {
    while (symbol_list.find(name) == symbol_list.end()) {
        if (this->parent != nullptr)
            return this->parent->lookupBinding(name);
        else
            throw LispError("Unbound variable " + name);
    }
    return symbol_list.at(name);
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

        if (Value::isList(vec[0])) 
            vec[0] = eval(vec[0]);

        if (auto name = vec[0]->asSymbol()) {
            if (SpecialForm::form_list.find(*name) !=
                SpecialForm::form_list.end()) {
                auto form = SpecialForm::form_list.at(*name);
                return form(ls->cdr()->toVector(), *this);
            }

            else {
                auto proc = lookupBinding(*name);
                std::vector<ValuePtr> args;
                if (Value::isList(ls->cdr()))
                    args = evalList(ls->cdr());
                else
                    args.push_back(eval(ls->cdr()));
                return apply(proc, args);
            }
        } 
        // ((return a lambda) x y)     // change asSymbol to asProc?
        else if (dynamic_pointer_cast<LambdaValue>(vec[0]) || dynamic_pointer_cast<BuiltinProcValue>(vec[0])) {
                std::vector<ValuePtr> args;
                if (Value::isList(ls->cdr()))
                    args = evalList(ls->cdr());
                else
                    args.push_back(eval(ls->cdr()));
                return apply(vec[0], args);     
        }

        else
            throw LispError("Not a procedure: " + vec[0]->toString());
    }

    else if (auto name = expr->asSymbol()) {
        return lookupBinding(*name);
    }

    else
        throw LispError("Unknown expression.");

    return std::make_shared<NilValue>();
}


// (define (a-plus-abs-b a b) ( (if (> b 0) + -) a b) )