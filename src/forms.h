#ifndef FORMS_H
#define FORMS_H

#include "./eval_env.h"
#include <string>
#include <unordered_map>
#include <vector>

using SpecialFormType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);

class SpecialForm {
private:
    static bool isVirtual(ValuePtr expr);

    static ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env);
    static ValuePtr lambdaForm(const std::vector<ValuePtr>& args, EvalEnv& env);
    static ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env);
    static ValuePtr ifForm(const std::vector<ValuePtr>& args, EvalEnv& env);
    static ValuePtr andForm(const std::vector<ValuePtr>& args, EvalEnv& env);
    static ValuePtr orForm(const std::vector<ValuePtr>& args, EvalEnv& env);

public:
    static const std::unordered_map<std::string, SpecialFormType*> form_list;
};

#endif