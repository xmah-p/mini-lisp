#include "./forms.h"

#include <memory>

#include "./error.h"


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

const std::unordered_map<std::string, SpecialFormType*> SpecialForm::form_list =
    {{"define", defineForm}
};
