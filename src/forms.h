#ifndef FORMS_H
#define FORMS_H

#include <string>
#include <unordered_map>
#include <vector>

#include "./eval_env.h"


using SpecialFormType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);

class SpecialForm {
private:
    static bool isVirtual(ValuePtr expr);  // return true iff expr == #f
    static SpecialFormType defineForm;
    static SpecialFormType lambdaForm;

    static SpecialFormType ifForm;
    static SpecialFormType andForm;
    static SpecialFormType orForm;
    static SpecialFormType beginForm;
    static SpecialFormType letForm;
    static SpecialFormType condForm;
    static SpecialFormType quoteForm;
    static SpecialFormType quasiquoteForm;
    static SpecialFormType unquoteForm;    // '(1 2) is as if (quote (1 2))

public:
    static const std::unordered_map<std::string, SpecialFormType*> form_list;
};

#endif