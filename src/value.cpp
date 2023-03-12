#include "./value.h"

#include <iomanip>
#include <sstream>
#include <typeinfo>

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

std::string NilValue::toString() const {
    return "()";
}

std::string SymbolValue::toString() const {
    return name;
}

void PairValue::toStringRecursive(std::string& res, const PairValue& pair) const {
    auto l_part{pair.l_part};
    auto r_part{pair.r_part};

    res.append(l_part->toString());

    if (typeid(*r_part) == typeid(NilValue)) {
        return;
    }
    else if (auto rp{dynamic_pointer_cast<const PairValue>(r_part)}) {
        rp->toStringRecursive(res, *rp);
    }
    else {
        res.append(r_part->toString());
    }
}

std::string PairValue::toString() const {
    std::string res{'('};
    toStringRecursive(res, *this);
    res.push_back(')');
    return res;
}