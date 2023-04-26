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
    static SpecialFormType defineForm;
    static SpecialFormType lambdaForm;
    static SpecialFormType quoteForm;
    static SpecialFormType ifForm;
    static SpecialFormType andForm;
    static SpecialFormType orForm;
    static SpecialFormType beginForm;
    static SpecialFormType letForm;

public:
    static const std::unordered_map<std::string, SpecialFormType*> form_list;
};

#endif