#ifndef FORMS_H
#define FORMS_H

#include "./builtins.h"
#include "./eval_env.h"

using SpecialFormType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);

namespace SpecialForm {
// using helper functions in builtins.h
using Builtins::checkArgNum, Builtins::numericalize, Builtins::vectorize;

SpecialFormType defineForm;
SpecialFormType lambdaForm;
SpecialFormType ifForm;
SpecialFormType andForm;
SpecialFormType orForm;
SpecialFormType beginForm;
SpecialFormType letForm;
SpecialFormType condForm;
SpecialFormType quoteForm;
SpecialFormType quasiquoteForm;
SpecialFormType unquoteForm;  // '(1 2) is as if (quote (1 2))

// extra
SpecialFormType setForm;
SpecialFormType loadForm;
SpecialFormType readForm;
SpecialFormType readLineForm;
SpecialFormType readEvalForm;
SpecialFormType assertForm;
SpecialFormType assertTrueForm;
SpecialFormType checkErrorForm;
SpecialFormType defineTestForm;
SpecialFormType runTestForm;



extern const std::unordered_map<std::string, SpecialFormType*> form_list;
};  // namespace SpecialForm

#endif