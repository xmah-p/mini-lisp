#ifndef FORMS_H
#define FORMS_H

#include "./eval_env.h"
#include <string>
#include <unordered_map>

using SpecialFormType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);

class SpecialForm {
private:
    static ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env);

public:
    static const std::unordered_map<std::string, SpecialFormType*> form_list;
};

#endif