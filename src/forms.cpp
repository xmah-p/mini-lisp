#include "./forms.h"

#include <algorithm>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "./error.h"


bool SpecialForm::isVirtual(ValuePtr expr) {
    if (auto cond = std::dynamic_pointer_cast<BooleanValue>(expr))
        if (!cond->getBool()) return true;
    return false;
}

ValuePtr SpecialForm::defineForm(const std::vector<ValuePtr>& args,
                                 EvalEnv& env) {
    if (args.size() < 2)
        throw LispError("Too few operands: " + std::to_string(args.size()) +
                        " < 2");

    // (define (double x) (+ x x) x)  <=>  (define double (lambda (x) (+ x x)))
    if (auto name = args[0]->asSymbol())
        env.symbol_list[*name] = env.eval(args[1]);

    else if (Value::isList(args[0])) {
        auto vec = args[0]->toVector();
        auto func = vec[0]->asSymbol();
        vec.erase(vec.begin());
        std::vector<ValuePtr> lambda_args{args};
        lambda_args[0] = Value::makeList(vec);
        auto lambda = lambdaForm(lambda_args, env);
        env.symbol_list[*func] =
            std::dynamic_pointer_cast<LambdaValue>(lambda);
    }

    else {
        throw LispError("Malformed define form: " + args[1]->toString());
    }
    return std::make_shared<NilValue>();
}

ValuePtr SpecialForm::lambdaForm(const std::vector<ValuePtr>& args,
                                 EvalEnv& env) {
    // (lambda (a b) ( (if (> b 0) + -) a b))
    if (args.size() < 2)
        throw LispError("Too few operands: " + std::to_string(args.size()) +
                        " < 2");
    std::vector<std::string> params;
    std::ranges::transform(args[0]->toVector(), std::back_inserter(params),
                           [](ValuePtr v) { return v->toString(); });

    std::vector<ValuePtr> body = args;
    body.erase(body.begin());
    return std::make_shared<LambdaValue>(params, body, env.shared_from_this());
}

ValuePtr SpecialForm::quoteForm(const std::vector<ValuePtr>& args,
                                EvalEnv& env) {
    return args[0];
}

ValuePtr SpecialForm::ifForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() < 2)
        throw LispError("Too few operands: " + std::to_string(args.size()) +
                        " < 2");
    if (isVirtual(env.eval(args[0]))) {
        if (args.size() < 3) return std::make_shared<NilValue>();
        return env.eval(args[2]);
    }
    return env.eval(args[1]);
}

ValuePtr SpecialForm::andForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (!args.empty()) {
        for (unsigned int i = 0; i != args.size(); ++i) {
            auto val = env.eval(args[i]);
            if (isVirtual(val)) return std::make_shared<BooleanValue>(false);
            if (i == args.size() - 1) return val;
        }
    }
    return std::make_shared<BooleanValue>(true);
}

ValuePtr SpecialForm::orForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (!args.empty()) {
        for (auto& i : args) {
            auto val = env.eval(i);
            if (isVirtual(val)) continue;
            return val;
        }
    }
    return std::make_shared<BooleanValue>(false);
}

const std::unordered_map<std::string, SpecialFormType*> SpecialForm::form_list{
    {"define", defineForm}, {"lambda", lambdaForm}, {"quote", quoteForm},
    {"if", ifForm},         {"and", andForm},       {"or", orForm}};
