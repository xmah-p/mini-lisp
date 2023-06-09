#include "./forms.h"

#include <algorithm>
#include <iostream>
#include <ranges>

#include "./boot.h"
#include "./error.h"

ValuePtr SpecialForm::defineForm(const std::vector<ValuePtr>& args,
                                 EvalEnv& env) {
    checkArgNum(args, 2);

    if (Value::isList(args[0])) {
        auto signature = args[0]->toVector();
        if (signature.empty())
            throw LispError("Malformed define form: " + args[0]->toString());

        std::vector<ValuePtr> lambda_args{args};
        lambda_args[0] =
            Value::makeList({signature.begin() + 1, signature.end()});

        auto lambda = lambdaForm(lambda_args, env);
        env.defineBinding(signature[0], lambda);
        return quoteForm({signature[0]}, env);
    }

    else {
        checkArgNum(args, 2, 2);
        env.defineBinding(args[0], env.eval(args[1]));
        return quoteForm({args[0]}, env);
    }
}

ValuePtr SpecialForm::lambdaForm(const std::vector<ValuePtr>& args,
                                 EvalEnv& env) {
    // (lambda (a b) ( (if (> b 0) + -) a b))
    checkArgNum(args, 2);

    std::vector<std::string> params;
    std::ranges::transform(args[0]->toVector(), std::back_inserter(params),
                           [](ValuePtr val) { return val->asSymbol(); });

    std::vector<ValuePtr> body(args.begin() + 1, args.end());
    return std::make_shared<LambdaValue>(params, body, env.shared_from_this());
}

ValuePtr SpecialForm::ifForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    checkArgNum(args, 2);

    if (Value::isVirtual(env.eval(args[0]))) {
        if (args.size() < 3) return std::make_shared<NilValue>();
        return env.eval(args[2]);
    }
    return env.eval(args[1]);
}

ValuePtr SpecialForm::andForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (!args.empty()) {
        for (std::size_t i = 0; i != args.size(); ++i) {
            auto val = env.eval(args[i]);
            if (Value::isVirtual(val))
                return std::make_shared<BooleanValue>(false);
            if (i == args.size() - 1) return val;
        }
    }
    return std::make_shared<BooleanValue>(true);
}

ValuePtr SpecialForm::orForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (!args.empty()) {
        for (auto& arg : args) {
            auto val = env.eval(arg);
            if (Value::isVirtual(val)) continue;
            return val;
        }
    }
    return std::make_shared<BooleanValue>(false);
}

ValuePtr SpecialForm::condForm(const std::vector<ValuePtr>& args,
                               EvalEnv& env) {
    for (std::size_t i = 0; i != args.size(); ++i) {
        auto clause = vectorize(args[i]);
        ValuePtr cond;
        if (clause[0]->toString() == "else") {
            if (i != args.size() - 1)
                throw LispError(
                    "Bad syntax: else clause must appear at the end");
            cond = std::make_shared<BooleanValue>(true);
        } else
            cond = env.eval(clause[0]);
        if (Value::isVirtual(cond)) continue;
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
    checkArgNum(args, 1);
    for (std::size_t i = 0; i != args.size(); ++i) {
        auto val = env.eval(args[i]);
        if (i == args.size() - 1) return env.eval(args[i]);
    }
    return std::make_shared<NilValue>();
}

ValuePtr SpecialForm::letForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    checkArgNum(args, 2);

    auto param_list = vectorize(args[0]);
    std::vector<std::string> names;
    std::vector<ValuePtr> values;

    for (auto& bind : param_list) {
        auto bind_vec = vectorize(bind);
        checkArgNum(bind_vec, 2, 2);

        names.push_back(bind_vec[0]->asSymbol());

        values.push_back(env.eval(bind_vec[1]));
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

    auto quoted = args[0]->toVector();
    if (Value::isSymbol(quoted[0]) && quoted[0]->asSymbol() == "unquote")
        return env.eval(quoted[1]);

    for (auto& expr : quoted) {
        if (Value::isList(expr) &&
            expr->toVector()[0]->asSymbol() == "unquote") {
            expr = env.eval(expr->toVector()[1]);
        }
    }
    return Value::makeList(quoted);
}

ValuePtr SpecialForm::unquoteForm(const std::vector<ValuePtr>& args,
                                  EvalEnv& env) {
    throw LispError("Cannot call unquote form outside quasiquote form");
}

// extra

ValuePtr SpecialForm::loadForm(const std::vector<ValuePtr>& args,
                               EvalEnv& env) {
    checkArgNum(args, 1, 1);

    std::string filename = args[0]->asString();
    fileMode(filename);
    return std::make_shared<NilValue>();
}

ValuePtr SpecialForm::readForm(const std::vector<ValuePtr>& args,
                               EvalEnv& env) {
    checkArgNum(args, 0, 0);

    return readParse(std::cin);
}

ValuePtr SpecialForm::readLineForm(const std::vector<ValuePtr>& args,
                                   EvalEnv& env) {
    return std::make_shared<StringValue>(readForm(args, env)->toString());
}

ValuePtr SpecialForm::readEvalForm(const std::vector<ValuePtr>& args,
                                   EvalEnv& env) {
    checkArgNum(args, 0, 0);

    return env.eval(readParse(std::cin));
}

ValuePtr SpecialForm::assertForm(const std::vector<ValuePtr>& args,
                                 EvalEnv& env) {
    checkArgNum(args, 1, 2);

    ValuePtr val = env.eval(args[0]);
    std::string msg = "";
    if (args.size() == 2) msg = args[1]->asString();

    try {
        if (Value::isVirtual(val)) throw TestFailure(msg);
    } catch (TestFailure& e) {
        std::cerr << "Assertion failed: (assert " + args[0]->toString() + ")"
                  << std::endl;
        if (msg != "") std::cerr << "Message: " + msg << std::endl;
        throw e;
    }
    return std::make_shared<BooleanValue>(true);
}

ValuePtr SpecialForm::assertTrueForm(const std::vector<ValuePtr>& args,
                                     EvalEnv& env) {
    checkArgNum(args, 1, 2);

    ValuePtr val = env.eval(args[0]);
    bool is_true = val->asBool();
    std::string msg = "";
    if (args.size() == 2) msg = args[1]->asString();

    try {
        if (!is_true) throw TestFailure(msg);
    } catch (TestFailure& e) {
        std::cerr << "Assertion failed: (assert-true " + args[0]->toString() +
                         ")"
                  << std::endl;
        if (msg != "") std::cerr << "Message: " + msg << std::endl;
        throw e;
    }
    return std::make_shared<BooleanValue>(true);
}

ValuePtr SpecialForm::checkErrorForm(const std::vector<ValuePtr>& args,
                                     EvalEnv& env) {
    checkArgNum(args, 1, 2);

    std::string msg = "";
    if (args.size() == 2) msg = args[1]->asString();

    try {
        ValuePtr val = env.eval(args[0]);
        std::cerr << "Check-error failed: (check-error " + args[0]->toString() +
                         ")"
                  << std::endl;
        if (msg != "") std::cerr << "Message: " + msg << std::endl;
    } catch (std::runtime_error& e) {
        return std::make_shared<BooleanValue>(true);
    }
    throw TestFailure(msg);
}

ValuePtr SpecialForm::defineTestForm(const std::vector<ValuePtr>& args,
                                     EvalEnv& env) {
    checkArgNum(args, 2);

    std::vector<ValuePtr> lambda_args{args};
    lambda_args[0] = Value::makeList({});
    auto test = lambdaForm(lambda_args, env);
    auto test_sym =
        std::make_shared<SymbolValue>(args[0]->toString() + "@TEST");
    env.defineBinding(test_sym, test);
    return quoteForm({args[0]}, env);
}

ValuePtr SpecialForm::runTestForm(const std::vector<ValuePtr>& args,
                                  EvalEnv& env) {
    checkArgNum(args, 1);
    for (auto& test : args) {
        try {
            std::cout << "Running test: " << test->toString() << std::endl;
            env.eval(Value::makeList({test}));
            std::cout << "Test passed\n" << std::endl;
        } catch (Error& e) {
            e.handle();
            std::cout << "Test failed: " << test->toString() + "\n"
                      << std::endl;
        }
    }
    return std::make_shared<NilValue>();
}

ValuePtr SpecialForm::runAllTestsForm(const std::vector<ValuePtr>& args,
                                      EvalEnv& env) {
    checkArgNum(args, 0, 0);
    auto tests = env.getAllTestsName();
    int passed = 0;
    for (auto& test : tests) {
        try {
            std::cout << "Running test: " << test->toString() << std::endl;
            env.eval(Value::makeList({test}));
            passed++;
            std::cout << "Test passed\n" << std::endl;
        } catch (Error& e) {
            e.handle();
            std::cout << "Test failed: " << test->toString() + "\n"
                      << std::endl;
        }
    }
    std::cout << "Tests passed: " + std::to_string(passed) + "/" +
                     std::to_string(tests.size())
              << std::endl;
    return std::make_shared<NilValue>();
}

extern const std::unordered_map<std::string, SpecialFormType*>
    SpecialForm::form_list{{"define", defineForm},
                           {"lambda", lambdaForm},
                           {"quote", quoteForm},
                           {"if", ifForm},
                           {"and", andForm},
                           {"or", orForm},
                           {"begin", beginForm},
                           {"let", letForm},
                           {"cond", condForm},
                           {"quasiquote", quasiquoteForm},
                           {"unquote", unquoteForm},
                           {"load", loadForm},
                           {"read", readForm},
                           {"read-line", readLineForm},
                           {"read-eval", readEvalForm},
                           {"assert", assertForm},
                           {"assert-true", assertTrueForm},
                           {"check-error", checkErrorForm},
                           {"define-test", defineTestForm},
                           {"run-test", runTestForm},
                           {"run-all-tests", runAllTestsForm}};
