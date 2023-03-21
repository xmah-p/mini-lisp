#ifndef VALUE_H
#define VALUE_H

#include <memory>
#include <optional>
#include <vector>
#include <string>

enum class ValueType {
    BOOLEAN,
    NUMERIC,
    STRING,
    NIL,
    SYMBOL,
    PAIR,
    BUILTIN_PROC
};

class Value;

using ValuePtr = std::shared_ptr<Value>;
using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&);

class Value {
private:
    ValueType type;

protected:
    Value(ValueType type) : type{type} {}

public:
    virtual ~Value() = 0;
    std::vector<ValuePtr> toVector();  // return vector storing the list
    std::optional<std::string> asSymbol() const;  // return its name if symbol
    std::optional<double> asNumber() const;    // return its value if number
    virtual std::string toString() const;

    static bool isNil(ValuePtr expr);
    static bool isSelfEvaluating(
        ValuePtr expr);  // return true if numeric / string / boolean
    static bool isList(ValuePtr expr);
    static ValuePtr makeList(std::vector<ValuePtr> lst);

    // Value 是抽象类 不能使用 make_shared<Value>()
};

class BooleanValue : public Value {
private:
    bool value;

public:
    BooleanValue(bool value) : Value(ValueType::BOOLEAN), value{value} {};
    std::string toString() const override;
};

class NumericValue : public Value {
private:
    double value;

public:
    NumericValue(double value) : Value(ValueType::NUMERIC), value{value} {}
    double getVal() const;
    std::string toString() const override;
};

class StringValue : public Value {
private:
    std::string str;

public:
    StringValue(const std::string str)
        : Value(ValueType::STRING), str{str} {}
    std::string toString() const override;
    std::string getStr() const;
};

class NilValue : public Value {
public:
    NilValue() : Value(ValueType::NIL) {}
    std::string toString() const override;
};

class SymbolValue : public Value {
private:
    const std::string name;

public:
    SymbolValue(const std::string value)
        : Value(ValueType::SYMBOL), name{value} {}
    std::string toString() const override;
};

class PairValue : public Value {
private:
    ValuePtr l_part;
    ValuePtr r_part;
    void toStringRecursive(std::string& res, const PairValue& pair) const;

public:
    PairValue(ValuePtr l_part, ValuePtr r_part)
        : Value(ValueType::PAIR), l_part{l_part}, r_part{r_part} {}
    ValuePtr car() const { return l_part; }
    ValuePtr cdr() const { return r_part; }
    std::string toString() const override;
};

class BuiltinProcValue : public Value {
private:
    BuiltinFuncType* func;

public:
    BuiltinProcValue(BuiltinFuncType* func)
        : Value(ValueType::BUILTIN_PROC), func{func} {}
    std::string toString() const override;
    BuiltinFuncType* getFunc() const;
};

#endif