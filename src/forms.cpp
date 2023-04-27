#include "./forms.h"

#include <algorithm>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <iostream>

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

    // (define (double x) (+ x x) x)  <=>  (define double (lambda (x) (+ x x)
    // x))
    if (auto name = args[0]->asSymbol()) {
        if (args.size() > 2)
            throw LispError(
                "Too many operands: " + std::to_string(args.size()) + " > 2");
        env.symbol_list[*name] = env.eval(args[1]);
    }

    else if (Value::isList(args[0])) {
        auto vec = args[0]->toVector();
        auto func = vec[0]->asSymbol();
        vec.erase(vec.begin());
        std::vector<ValuePtr> lambda_args{args};
        lambda_args[0] = Value::makeList(vec);
        auto lambda = lambdaForm(lambda_args, env);
        env.symbol_list[*func] = std::dynamic_pointer_cast<LambdaValue>(lambda);
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
    std::ranges::transform(
        args[0]->toVector(), std::back_inserter(params), [](ValuePtr v) {
            return v->asSymbol() == std::nullopt
                       ? throw LispError(
                             "Expect symbol in Lambda parameter, found " +
                             v->toString())
                       : *v->asSymbol();
        });

    std::vector<ValuePtr> body(args.begin() + 1, args.end());
    return std::make_shared<LambdaValue>(params, body, env.shared_from_this());
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
        for (std::size_t i = 0; i != args.size(); ++i) {
            auto val = env.eval(args[i]);
            if (isVirtual(val)) return std::make_shared<BooleanValue>(false);
            if (i == args.size() - 1) return val;
        }
    }
    return std::make_shared<BooleanValue>(true);
}

ValuePtr SpecialForm::orForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (!args.empty()) {
        for (auto& arg : args) {
            auto val = env.eval(arg);
            if (isVirtual(val)) continue;
            return val;
        }
    }
    return std::make_shared<BooleanValue>(false);
}

ValuePtr SpecialForm::condForm(const std::vector<ValuePtr>& args,
                               EvalEnv& env) {
    for (std::size_t i = 0; i != args.size(); ++i) {
        if (!Value::isList(args[i]))
            throw LispError("Malformed list: expected pair or nil, got " +
                            args[i]->toString());
        auto clause = args[i]->toVector();
        ValuePtr cond;
        if (clause[0]->toString() == "else") {
            if (i != args.size() - 1)
                throw LispError(
                    "Bad syntax: else clause must appear at the end");
            cond = std::make_shared<BooleanValue>(true);
        } else
            cond = env.eval(clause[0]);
        if (isVirtual(cond)) continue;
        if (clause.size() == 1) return cond;
        for (std::size_t j = 1; j != clause.size(); ++j) {
            if (j == clause.size() - 1) return env.eval(clause[j]);
            env.eval(clause[j]);
        }
    }
    return std::make_shared<NilValue>();
}

ValuePtr SpecialForm::beginForm(const std::vector<ValuePtr>& args,
                                EvalEnv& env) {
    if (args.size() < 1)
        throw LispError("Too few operands: " + std::to_string(args.size()) +
                        " < 1");
    for (std::size_t i = 0; i != args.size(); ++i) {
        auto val = env.eval(args[i]);
        if (i == args.size() - 1) return env.eval(args[i]);
    }
    return std::make_shared<NilValue>();
}

ValuePtr SpecialForm::letForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    // (let( (x 42)(y 56) ) (+ x y) x)
    // is equal to
    // ((lambda (x y) (+ x y) x) 42 56)
    if (args.size() < 2)
        throw LispError("Too few operands: " + std::to_string(args.size()) +
                        " < 2");
    if (!Value::isList(args[0]))
        throw LispError("Malformed list: expected pair or nil, got " +
                        args[0]->toString());
    auto param_list = args[0]->toVector();
    std::vector<std::string> names;
    std::vector<ValuePtr> values;
    for (auto& bind : param_list) {
        if (!Value::isList(bind))
            throw LispError("Malformed list: expected pair or nil, got " +
                            bind->toString());
        auto vec = bind->toVector();
        if (vec.size() < 2)
            throw LispError("Too few operands: " + std::to_string(vec.size()) +
                            " < 2");
        if (vec.size() > 2)
            throw LispError("Too many operands: " + std::to_string(vec.size()) +
                            " > 2");
        names.push_back(
            vec[0]->asSymbol() == std::nullopt
                ? throw LispError("Expected let binding name, got " +
                                  vec[0]->toString())
                : *vec[0]->asSymbol());
        values.push_back(vec[1]);
    }
    std::vector<ValuePtr> body(args.begin() + 1, args.end());
    auto lambda =
        std::make_shared<LambdaValue>(names, body, env.shared_from_this());
    return lambda->apply(values);
}

ValuePtr SpecialForm::quoteForm(const std::vector<ValuePtr>& args,
                                EvalEnv& env) {
    return args[0];
}

ValuePtr SpecialForm::quasiquoteForm(const std::vector<ValuePtr>& args,
                                     EvalEnv& env) {
    if (!Value::isList(args[0])) return args[0];
    auto vec = args[0]->toVector();
    for (auto& expr : vec) {
        if (Value::isList(expr) &&
            expr->toVector()[0]->asSymbol() == "unquote") {
            expr = env.eval(expr->toVector()[1]);
        }
    }
    return Value::makeList(vec);
}

ValuePtr SpecialForm::unquoteForm(const std::vector<ValuePtr>& args,
                                  EvalEnv& env) {
    return std::make_shared<NilValue>();
}

const std::unordered_map<std::string, SpecialFormType*> SpecialForm::form_list{
    {"define", defineForm},  {"lambda", lambdaForm},
    {"quote", quoteForm},    {"if", ifForm},
    {"and", andForm},        {"or", orForm},
    {"begin", beginForm},    {"let", letForm},
    {"cond", condForm},      {"quasiquote", quasiquoteForm},
    {"unquote", unquoteForm}};
