#ifndef BUILTINS_H
#define BUILTINS_H

#include <unordered_map>
#include <vector>

#include "./value.h"

class Builtins {
    friend class BuiltinProcValue;

private:
    // calc
    static ValuePtr add(const std::vector<ValuePtr>& params);

    // type
    static ValuePtr isAtom(const std::vector<ValuePtr>& params);
    static ValuePtr isBoolean(const std::vector<ValuePtr>& params);
    static ValuePtr isInteger(const std::vector<ValuePtr>& params);
    static ValuePtr isList(const std::vector<ValuePtr>& params);
    static ValuePtr isNumber(const std::vector<ValuePtr>& params);
    static ValuePtr isNull(const std::vector<ValuePtr>& params);
    static ValuePtr isPair(const std::vector<ValuePtr>& params);
    static ValuePtr isProcedure(const std::vector<ValuePtr>& params);
    static ValuePtr isString(const std::vector<ValuePtr>& params);
    static ValuePtr isSymbol(const std::vector<ValuePtr>& params);

    // core
    static ValuePtr display(const std::vector<ValuePtr>& params);
    static ValuePtr newline(const std::vector<ValuePtr>& params);
    static ValuePtr print(const std::vector<ValuePtr>& params);

public:
    static void initSymbolList();
    static void exit(const std::vector<ValuePtr>& params);
};

#endif