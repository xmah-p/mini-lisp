#ifndef VALUE_H
#define VALUE_H

#include <string>

class EvalEnv;

enum class ValueType {
    BOOLEAN,
    NUMERIC,
    STRING,
    NIL,
    SYMBOL,
    PAIR,
    BUILTIN_PROC,
    LAMBDA
};

class Value;

using ValuePtr = std::shared_ptr<Value>;
using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);

class Value {
private:
    ValueType type;

protected:
    Value(ValueType type) : type{type} {}

public:
    virtual ~Value() = 0;
    std::vector<ValuePtr>
    toVector();  // list to vector, empty vector if nilValue
    std::optional<std::string> asSymbol() const;
    std::optional<double> asNumber() const;
    std::string asString() const;
    virtual std::string toString() const;

    static bool isNil(ValuePtr expr);
    static bool isSelfEvaluating(
        ValuePtr expr);  // return true if numeric / string / boolean
    static bool isPair(ValuePtr expr);
    static bool isList(ValuePtr expr);
    static bool isProcedure(ValuePtr expr);

    static ValuePtr makeList(const std::vector<ValuePtr>& lst);
};

class BooleanValue : public Value {
private:
    bool value;

public:
    BooleanValue(bool value) : Value(ValueType::BOOLEAN), value{value} {};
    bool getBool() const;
    std::string toString() const final;
};

class NumericValue : public Value {
private:
    double value;

public:
    NumericValue(double value) : Value(ValueType::NUMERIC), value{value} {}
    double getVal() const;
    std::string toString() const final;
};

class StringValue : public Value {
private:
    std::string str;

public:
    StringValue(const std::string str) : Value(ValueType::STRING), str{str} {}
    std::string toString() const final;
    std::string getStr() const;
};

class NilValue : public Value {
public:
    NilValue() : Value(ValueType::NIL) {}
    std::string toString() const final;
};

class SymbolValue : public Value {
private:
    const std::string name;

public:
    SymbolValue(const std::string value)
        : Value(ValueType::SYMBOL), name{value} {}
    std::string toString() const final;
};

class PairValue : public Value {
private:
    ValuePtr l_part;
    ValuePtr r_part;
    void toStringRecursive(std::string& res, const PairValue& pair) const;

public:
    PairValue(ValuePtr l_part, ValuePtr r_part)
        : Value(ValueType::PAIR), l_part{l_part}, r_part{r_part} {}
    ValuePtr car() const {
        return l_part;
    }
    ValuePtr cdr() const {
        return r_part;
    }
    std::string toString() const final;
};

class BuiltinProcValue : public Value {
private:
    std::function<BuiltinFuncType> func;

public:
    BuiltinProcValue(std::function<BuiltinFuncType> func)
        : Value(ValueType::BUILTIN_PROC), func{func} {}
    std::string toString() const final;
    std::function<BuiltinFuncType> getFunc() const;
};

class LambdaValue : public Value {
private:
    std::vector<std::string> params;
    std::vector<ValuePtr> body;
    std::shared_ptr<EvalEnv> envPtr;

public:
    LambdaValue(std::vector<std::string> params, std::vector<ValuePtr> body,
                std::shared_ptr<EvalEnv> envPtr)
        : Value(ValueType::LAMBDA),
          params{params},
          body{body},
          envPtr{envPtr} {}

    // eval args by envPtr->env(), then apply them to lambda
    ValuePtr apply(const std::vector<ValuePtr>& args) const;
    std::string toString() const final;
};

#endif