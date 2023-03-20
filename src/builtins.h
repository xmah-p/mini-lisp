#ifndef BUILTINS_H
#define BUILTINS_H

#include <unordered_map>

#include "./value.h"

class Builtins {
    friend class BuiltinProcValue;

private:
    static ValuePtr add(const std::vector<ValuePtr>& params);

public:
    static void initSymbolList();
};

#endif