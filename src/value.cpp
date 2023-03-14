#include "./value.h"

#include <iomanip>
#include <sstream>
#include <typeinfo>

std::string Value::toString() const { return "UNKNOWN VALUE"; }

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
    res.push_back(' ');

    if (typeid(*r_part) == typeid(NilValue)) {
        return;
    } else if (auto rp{dynamic_pointer_cast<const PairValue>(r_part)}) {
        rp->toStringRecursive(res, *rp);
    } else {
        res.append(". ");
        res.append(r_part->toString());
    }
}

std::string PairValue::toString() const {
    std::string res{"("};
    toStringRecursive(res, *this);
    res.push_back(')');
    return res;
}

ValuePtr PairValue::makeList(std::deque<ValuePtr> lst) {
    if (lst.size() == 1) {
        return std::make_shared<PairValue>(lst.front(),
                                           std::make_shared<NilValue>());
    } else {
        auto car = lst.front();
        lst.pop_front();
        auto cdr = makeList(lst);
        return std::make_shared<PairValue>(car, cdr);
    }
}
