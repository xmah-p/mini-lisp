#ifndef VALUE_H
#define VALUE_H

#include <deque>
#include <memory>
#include <vector>
#include <optional>

enum class ValueType { BOOLEAN, NUMERIC, STRING, NIL, SYMBOL, PAIR };

class Value;

using ValuePtr = std::shared_ptr<Value>;

class Value {
private:
    ValueType type;

protected:
    Value(ValueType type) : type{type} {}

public:
    Value() = default;  // not using yet

    std::vector<ValuePtr> toVector();
    std::optional<std::string> asSymbol() const;
    virtual ~Value() = 0;
    virtual std::string toString() const;
    static bool isNil(ValuePtr expr);
    static bool isSelfEvaluating(ValuePtr expr);
    static bool isList(ValuePtr expr);

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
    std::string toString() const override;
};

class StringValue : public Value {
private:
    std::string value;

public:
    StringValue(const std::string value)
        : Value(ValueType::STRING), value{value} {}
    std::string toString() const override;
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
    static ValuePtr makeList(std::deque<ValuePtr> lst);
};

#endif