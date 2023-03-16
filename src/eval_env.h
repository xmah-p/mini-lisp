#ifndef EVAL_ENV_H
#define EVAL_ENV_H

#include <unordered_map>

#include "./value.h"
#include "./builtins.h"

class EvalEnv {
public:
    ValuePtr eval(ValuePtr expr);
    std::vector<ValuePtr> evalList(ValuePtr ls);
    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr> args);
    static std::unordered_map<std::string, ValuePtr> symbol_list;
};

#endif