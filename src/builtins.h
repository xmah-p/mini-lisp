#ifndef BUILTINS_H
#define BUILTINS_H

#include <functional>
#include <unordered_map>
#include <vector>

#include "./value.h"

namespace Builtins {
// calc
ValuePtr add(const std::vector<ValuePtr>& params);
ValuePtr subtract(const std::vector<ValuePtr>& params);
ValuePtr multiply(const std::vector<ValuePtr>& params);
ValuePtr divide(const std::vector<ValuePtr>& params);
ValuePtr abs(const std::vector<ValuePtr>& params);

// pair and list
ValuePtr car(const std::vector<ValuePtr>& params);
ValuePtr cdr(const std::vector<ValuePtr>& params);

// type
ValuePtr isAtom(const std::vector<ValuePtr>& params);
ValuePtr isBoolean(const std::vector<ValuePtr>& params);
ValuePtr isInteger(const std::vector<ValuePtr>& params);
ValuePtr isList(const std::vector<ValuePtr>& params);
ValuePtr isNumber(const std::vector<ValuePtr>& params);
ValuePtr isNull(const std::vector<ValuePtr>& params);
ValuePtr isPair(const std::vector<ValuePtr>& params);
ValuePtr isProcedure(const std::vector<ValuePtr>& params);
ValuePtr isString(const std::vector<ValuePtr>& params);
ValuePtr isSymbol(const std::vector<ValuePtr>& params);

// core
// std::function<BuiltinFuncType> apply;  // apply and eval are defined
// std::function<BuiltinFuncType> eval;   // as lambda expr in eval_env.cpp
ValuePtr display(const std::vector<ValuePtr>& params);
ValuePtr newline(const std::vector<ValuePtr>& params);
ValuePtr print(const std::vector<ValuePtr>& params);
ValuePtr exit(const std::vector<ValuePtr>& params);

// comp
ValuePtr greater(const std::vector<ValuePtr>& params);
ValuePtr lesser(const std::vector<ValuePtr>& params);
ValuePtr equalNum(const std::vector<ValuePtr>& params);
ValuePtr greaterOrEqual(const std::vector<ValuePtr>& params);
ValuePtr lesserOrEqual(const std::vector<ValuePtr>& params);
ValuePtr isZero(const std::vector<ValuePtr>& params);
};  // namespace Builtins

#endif