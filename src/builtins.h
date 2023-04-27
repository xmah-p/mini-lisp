#ifndef BUILTINS_H
#define BUILTINS_H

#include <functional>
#include <unordered_map>
#include <vector>

#include "./value.h"

namespace Builtins {
// calc
BuiltinFuncType add;
BuiltinFuncType subtract;
BuiltinFuncType multiply;
BuiltinFuncType divide;
BuiltinFuncType abs;

// pair and list
BuiltinFuncType car;
BuiltinFuncType cdr;

// type complete
BuiltinFuncType isAtom;
BuiltinFuncType isBoolean;
BuiltinFuncType isInteger;
BuiltinFuncType isList;
BuiltinFuncType isNumber;
BuiltinFuncType isNull;
BuiltinFuncType isPair;
BuiltinFuncType isProcedure;
BuiltinFuncType isString;
BuiltinFuncType isSymbol;

// core complete
// std::function<BuiltinFuncType> apply;  // apply and eval are defined
// std::function<BuiltinFuncType> eval;   // as lambda expr in eval_env.cpp
BuiltinFuncType display;
BuiltinFuncType newline;
BuiltinFuncType displayln;
BuiltinFuncType print;
BuiltinFuncType exit;
BuiltinFuncType error;

// comp complete
BuiltinFuncType isEq;
BuiltinFuncType isEqualValue;
BuiltinFuncType isNot;
BuiltinFuncType greater;         // >
BuiltinFuncType lesser;          // <
BuiltinFuncType equalNum;        // =
BuiltinFuncType greaterOrEqual;  // >=
BuiltinFuncType lesserOrEqual;   // <=
BuiltinFuncType isZero;
BuiltinFuncType isEven;
BuiltinFuncType isOdd;
};  // namespace Builtins

#endif