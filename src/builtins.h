#ifndef BUILTINS_H
#define BUILTINS_H

#include <unordered_map>
#include <vector>

#include "./value.h"

class Builtins {
    friend class BuiltinProcValue;

private:
    static ValuePtr add(const std::vector<ValuePtr>& params);
    static ValuePtr display(const std::vector<ValuePtr>& params);
    static ValuePtr newline(const std::vector<ValuePtr>& params);
    static ValuePtr print(const std::vector<ValuePtr>& params);

public:
    static void initSymbolList();
    static void exit(const std::vector<ValuePtr>& params);
};

#endif