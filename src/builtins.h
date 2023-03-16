#ifndef BUILTINS_H
#define BUILTINS_H

#include <unordered_map>

#include "./error.h"
#include "./eval_env.h"
#include "./value.h"

class Builtins {
    friend class BuiltinProcValue;

private:
    static ValuePtr add(const std::vector<ValuePtr>& params);

public:
    static void initSymbolList();
};

#endif