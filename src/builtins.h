#ifndef BUILTINS_H
#define BUILTINS_H

#include "./value.h"

namespace Builtins {

// helper functions
void checkArgNum(const std::vector<ValuePtr>& params, std::size_t min,
                 std::size_t max = std::numeric_limits<std::size_t>::max());
std::vector<ValuePtr> vectorize(const ValuePtr& ls);
std::vector<double> numericalize(const std::vector<ValuePtr>& vals);


// calc 9
BuiltinFuncType add;
BuiltinFuncType subtract;
BuiltinFuncType multiply;
BuiltinFuncType divide;
BuiltinFuncType abs;
BuiltinFuncType expt;
BuiltinFuncType quotient;
BuiltinFuncType remainder;
BuiltinFuncType modulo;

// pair and list 9
BuiltinFuncType car;
BuiltinFuncType cdr;
BuiltinFuncType append;
BuiltinFuncType cons;
BuiltinFuncType length;
BuiltinFuncType list;
BuiltinFuncType map;
BuiltinFuncType filter;
BuiltinFuncType reduce;

// type 10
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

// core 8
BuiltinFuncType apply;
BuiltinFuncType eval;
BuiltinFuncType display;
BuiltinFuncType newline;
BuiltinFuncType displayln;
BuiltinFuncType print;
BuiltinFuncType exit;
BuiltinFuncType error;

// comp 11
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

// extra
BuiltinFuncType max;
BuiltinFuncType min;
BuiltinFuncType listRef;
BuiltinFuncType listTail;
BuiltinFuncType forEach;
BuiltinFuncType listReverse;
BuiltinFuncType member;
BuiltinFuncType numberToString;
BuiltinFuncType stringToNumber;
BuiltinFuncType makeStr;
BuiltinFuncType strRef;
BuiltinFuncType strLength;
BuiltinFuncType strAppend;
BuiltinFuncType strCopy;
BuiltinFuncType subStr;


// 51 std builtin forms, including 4 overloads
extern const std::unordered_map<std::string, BuiltinFuncType*> builtin_forms;
};  // namespace Builtins

#endif