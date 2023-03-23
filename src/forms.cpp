#include "./forms.h"

#include <memory>
#include <string>

#include "./error.h"

bool SpecialForm::isVirtual(ValuePtr expr) {
    if (auto cond = std::dynamic_pointer_cast<BooleanValue>(expr))
        if (!cond->getBool()) return true;
    return false;
}

ValuePtr SpecialForm::defineForm(const std::vector<ValuePtr>& args,
                                 EvalEnv& env) {
    if (args.size() > 2)
        throw LispError("Too many operands: " + std::to_string(args.size()) +
                        " > 2");
    if (args.size() < 2)
        throw LispError("Too few operands: " + std::to_string(args.size()) +
                        " < 2");
    if (auto name = args[0]->asSymbol())
        EvalEnv::symbol_list[*name] = env.eval(args[1]);
    else
        throw LispError("Unimplemented");
    return std::make_shared<NilValue>();
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
    {"define", defineForm},
    {"quote", quoteForm},
    {"if", ifForm},
    {"and", andForm},
    {"or", orForm}};
