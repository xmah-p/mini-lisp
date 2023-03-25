#ifndef EVAL_ENV_H
#define EVAL_ENV_H

#include <string>
#include <unordered_map>

#include "./value.h"


class EvalEnv {
public:
    EvalEnv();
    ValuePtr eval(ValuePtr expr);
    std::vector<ValuePtr> evalList(ValuePtr ls);
    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr> args);
    std::unordered_map<std::string, ValuePtr> symbol_list;
};

#endif