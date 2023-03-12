#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <memory>

enum class ValueType { BOOLEAN, NUMERIC, STRING, NIL, SYMBOL, PAIR };

class Value {
private:
    ValueType type;

protected:
    Value(ValueType type) : type{type} {}

public:
    virtual ~Value() = default;
    virtual std::string toString() const;
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
    std::shared_ptr<Value> l_part;
    std::shared_ptr<Value> r_part;
    void toStringRecursive(std::string& res, const PairValue& pair) const;

public:
    PairValue(std::shared_ptr<Value> l_part, std::shared_ptr<Value> r_part)
        : Value(ValueType::PAIR), l_part{l_part}, r_part{r_part} {}
    std::string toString() const override;

};

#endif