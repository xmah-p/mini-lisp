#include "./value.h"

#include <iomanip>
#include <iostream>  // debug
#include <sstream>

#include "./error.h"
#include "./eval_env.h"

Value::~Value() {}

std::vector<ValuePtr> Value::toVector() const {
    std::vector<ValuePtr> vec;
    if (this->type == ValueType::NIL) return vec;
    if (auto pr = dynamic_cast<const PairValue*>(this)) {
        vec.push_back(pr->car());
        auto rest = pr->cdr()->toVector();
        vec.insert(vec.end(), rest.begin(), rest.end());
        return vec;
    }
    throw TypeError(this->toString() + " is not a list");
}

bool Value::asBool() const {
    if (auto num = dynamic_cast<const BooleanValue*>(this))
        return num->getVal();
    throw TypeError(this->toString() + " is not a boolean!");
}

double Value::asNumber() const {
    if (auto num = dynamic_cast<const NumericValue*>(this))
        return num->getVal();
    throw TypeError(this->toString() + " is not a number!");
}

std::string Value::asString() const {
    if (auto str = dynamic_cast<const StringValue*>(this)) return str->getVal();
    throw TypeError(this->toString() + " is not a string!");
}

std::string Value::asSymbol() const {
    if (auto sym = dynamic_cast<const SymbolValue*>(this))
        return sym->toString();
    throw TypeError(this->toString() + " is not a symbol!");
}

bool Value::isBoolean(ValuePtr expr) {
    return typeid(*expr) == typeid(BooleanValue);
}

bool Value::isNumeric(ValuePtr expr) {
    return typeid(*expr) == typeid(NumericValue);
}

bool Value::isString(ValuePtr expr) {
    return typeid(*expr) == typeid(StringValue);
}

bool Value::isSymbol(ValuePtr expr) {
    return typeid(*expr) == typeid(SymbolValue);
}

bool Value::isNil(ValuePtr expr) {
    return typeid(*expr) == typeid(NilValue);
}

bool Value::isPair(ValuePtr expr) {
    return typeid(*expr) == typeid(PairValue);
}

bool Value::isList(ValuePtr expr) {
    if (auto pr = dynamic_cast<const PairValue*>(expr.get()))
        return isList(pr->cdr());
    if (typeid(*expr) == typeid(NilValue)) return true;
    return false;
}

bool Value::isProcedure(ValuePtr expr) {
    return typeid(*expr) == typeid(LambdaValue) ||
           typeid(*expr) == typeid(BuiltinProcValue);
}

bool Value::isSelfEvaluating(ValuePtr expr) {
    return typeid(*expr) == typeid(NumericValue) ||
           typeid(*expr) == typeid(BooleanValue) ||
           typeid(*expr) == typeid(StringValue);
}

bool Value::isVirtual(ValuePtr expr) {
    if (auto boolean = dynamic_cast<const BooleanValue*>(expr.get()))
        return !boolean->getVal();
    return false;
}

std::string Value::toString() const {
    return "";
}

std::string BooleanValue::toString() const {
    return value == true ? "#t" : "#f";
}

bool BooleanValue::getVal() const {
    return value;
}

double NumericValue::getVal() const {
    return value;
}

std::string NumericValue::toString() const {
    return fmod(value, 1.0) == 0.0 ? std::to_string(static_cast<int>(value))
                                   : std::to_string(value);
}

std::string StringValue::toString() const {
    std::ostringstream oss;
    oss << std::quoted(str);
    return oss.str();
}

std::string StringValue::getVal() const {
    return str;
}

std::string NilValue::toString() const {
    return "()";
}

std::string SymbolValue::toString() const {
    return name;
}

void PairValue::toStringRecursive(std::string& res,
                                  const PairValue& pair) const {
    auto l_part{pair.l_part};
    auto r_part{pair.r_part};

    res.append(l_part->toString());

    if (isNil(r_part))
        return;
    else if (auto rp = dynamic_pointer_cast<PairValue>(r_part)) {
        res.push_back(' ');
        rp->toStringRecursive(res, *rp);
    } else {
        res.append(" . ");
        res.append(r_part->toString());
    }
}

std::string PairValue::toString() const {
    std::string res{"("};
    toStringRecursive(res, *this);
    res.push_back(')');
    return res;
}

ValuePtr Value::makeList(const std::vector<ValuePtr>& lst) {
    if (lst.empty()) return std::make_shared<NilValue>();

    return std::make_shared<PairValue>(lst.front(),
                                       makeList({lst.begin() + 1, lst.end()}));
}

std::string BuiltinProcValue::toString() const {
    return "#<procedure>";
}

std::function<BuiltinFuncType> BuiltinProcValue::getVal() const {
    return func;
}

ValuePtr LambdaValue::apply(const std::vector<ValuePtr>& args) const {
    auto env = this->envPtr->EvalEnv::createChild(this->params, args);
    for (std::size_t i = 0; i < this->body.size() - 1; ++i) {
        env->eval(this->body[i]);
    }
    return env->eval(this->body.back());
}

std::string LambdaValue::toString() const {
    return "#<procedure>";
}
