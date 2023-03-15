#include "./value.h"

#include <iomanip>
#include <sstream>
#include <algorithm>

#include "./error.h"

Value::~Value() {}

std::vector<ValuePtr> Value::toVector() {
    auto pr = dynamic_cast<PairValue*>(this);
    std::vector<ValuePtr> vec;
    do {
        vec.push_back(pr->car());
    } while (pr = dynamic_cast<PairValue*>(pr->cdr().get()));
    return vec;
}

std::optional<std::string> Value::asSymbol() const {
    if (auto sym = dynamic_cast<const SymbolValue*>(this))
        return sym->toString();
    return std::nullopt;
}

bool Value::isNil(ValuePtr expr) {
    if (dynamic_pointer_cast<NilValue>(expr)) return true;
    return false;
}

bool Value::isSelfEvaluating(ValuePtr expr) {
    if (dynamic_pointer_cast<NumericValue>(expr) ||
        dynamic_pointer_cast<StringValue>(expr) ||
        dynamic_pointer_cast<BooleanValue>(expr))
        return true;
    else
        return false;
}

bool Value::isList(ValuePtr expr) {
    if (dynamic_pointer_cast<PairValue>(expr)) return true;
    return false;
}

std::string Value::toString() const { return ""; }

std::string BooleanValue::toString() const {
    return value == true ? "#t" : "#f";
}

std::string NumericValue::toString() const {
    return (value - static_cast<int>(value)) == 0
               ? std::to_string(static_cast<int>(value))
               : std::to_string(value);
}

std::string StringValue::toString() const {
    std::ostringstream oss;
    oss << std::quoted(value);
    return oss.str();
}

std::string NilValue::toString() const { return "()"; }

std::string SymbolValue::toString() const { return name; }

void PairValue::toStringRecursive(std::string& res,
                                  const PairValue& pair) const {
    auto l_part{pair.l_part};
    auto r_part{pair.r_part};

    res.append(l_part->toString());

    if (dynamic_pointer_cast<NilValue>(r_part)) 
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

ValuePtr Value::makeList(std::vector<ValuePtr> lst) {
    std::reverse(lst.begin(), lst.end());
    if (lst.size() == 1) {
        return std::make_shared<PairValue>(lst.front(),
                                           std::make_shared<NilValue>());
    } else {
        auto car = lst.back();
        lst.pop_back();
        auto cdr = makeList(lst);
        return std::make_shared<PairValue>(car, cdr);
    }
}
