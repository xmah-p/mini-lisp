#include "./eval_env.h"


#include "./builtins.h"
#include "./error.h"
#include "./forms.h"

std::shared_ptr<EvalEnv> EvalEnv::createGlobal() {
    auto global = std::shared_ptr<EvalEnv>(new EvalEnv);

    for (auto&& [name, func] : Builtins::builtin_forms)
        global->symbol_list[name] = std::make_shared<BuiltinProcValue>(func);

    return global;
}

std::shared_ptr<EvalEnv> EvalEnv::createChild(
    const std::vector<std::string>& params, const std::vector<ValuePtr>& args) {

    auto child = std::shared_ptr<EvalEnv>(new EvalEnv(*this));
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
        return func->getFunc()(args, *this);
    } else if (auto func = std::dynamic_pointer_cast<LambdaValue>(proc)) {
        return func->apply(args);
    } else
        throw LispError("Not a procedure: " + proc->toString());
}

void EvalEnv::defineBinding(ValuePtr name, ValuePtr val) {
    auto sym = name->asSymbol();
    if (sym == std::nullopt)
        throw LispError(name->toString() + " is not a identifier");
    this->symbol_list[*sym] = val;
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

    else if (auto name = expr->asSymbol()) {
        return lookupBinding(*name);
    }

    else if (Value::isList(expr)) {
        auto ls = dynamic_pointer_cast<PairValue>(expr);
        std::vector<ValuePtr> vec = ls->toVector();

        if (Value::isList(vec[0])) vec[0] = eval(vec[0]);

        if (auto name = vec[0]->asSymbol()) {
            if (SpecialForm::form_list.find(*name) !=
                SpecialForm::form_list.end()) {
                // don't eval arguments here, eval them inside special forms
                auto form = SpecialForm::form_list.at(*name);
                return form(ls->cdr()->toVector(), *this);
            } else {
                auto proc = lookupBinding(*name);
                std::vector<ValuePtr> args;
                if (Value::isList(ls->cdr()))
                    args = evalList(ls->cdr());
                else
                    args.push_back(eval(ls->cdr()));
                return apply(proc, args);
            }
        } else if (dynamic_pointer_cast<LambdaValue>(vec[0]) ||
                   dynamic_pointer_cast<BuiltinProcValue>(vec[0])) {
            std::vector<ValuePtr> args;
            if (Value::isList(ls->cdr()))
                args = evalList(ls->cdr());
            else
                args.push_back(eval(ls->cdr()));
            return apply(vec[0], args);
        } else
            throw LispError("Not a procedure: " + vec[0]->toString());
    }

    else if (Value::isPair(expr))
        throw LispError("Malformed list: " + expr->toString());

    else
        throw LispError("Unknown expression: " + expr->toString());
}
