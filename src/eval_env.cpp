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
    if (auto func = dynamic_cast<BuiltinProcValue*>(proc.get())) {
        return func->getVal()(args, *this);
    } else if (auto func = dynamic_cast<LambdaValue*>(proc.get())) {
        return func->apply(args);
    } else
        throw TypeError(proc->toString() + "is not a procedure");
}

void EvalEnv::defineBinding(ValuePtr name, ValuePtr val) {
    std::string sym = name->asSymbol();
    this->symbol_list[sym] = val;
}

ValuePtr& EvalEnv::lookupBinding(std::string name) {
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

    else if (Value::isSymbol(expr)) {
        return lookupBinding(expr->asSymbol());
    }

    else if (Value::isList(expr)) {
        auto ls = dynamic_pointer_cast<PairValue>(expr);
        std::vector<ValuePtr> vec = ls->toVector();

        if (Value::isList(vec[0])) vec[0] = eval(vec[0]);

        if (Value::isSymbol(vec[0])) {
            std::string name = vec[0]->asSymbol();
            if (SpecialForm::form_list.find(name) !=
                SpecialForm::form_list.end()) {
                // arguments not eval here, eval them inside special forms
                auto form = SpecialForm::form_list.at(name);
                return form(ls->cdr()->toVector(), *this);
            } else {
                auto proc = lookupBinding(name);
                std::vector<ValuePtr> args;
                if (Value::isList(ls->cdr()))
                    args = evalList(ls->cdr());
                else
                    args.push_back(eval(ls->cdr()));
                return apply(proc, args);
            }
        } else if (Value::isProcedure(vec[0])) {
            std::vector<ValuePtr> args;
            if (Value::isList(ls->cdr()))
                args = evalList(ls->cdr());
            else
                args.push_back(eval(ls->cdr()));
            return apply(vec[0], args);
        } else
            throw TypeError(vec[0]->toString() + "is not a procedure");
    }

    else if (Value::isPair(expr))
        throw LispError("Malformed list: " + expr->toString());

    else
        throw LispError("Unknown expression: " +
                        expr->toString());  // dead code
}
