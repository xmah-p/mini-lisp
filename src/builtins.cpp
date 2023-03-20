#include "./builtins.h"
#include "./error.h"
#include "./eval_env.h"


ValuePtr Builtins::add(const std::vector<ValuePtr>& params) {
    double total{0};
    for (auto& i : params) {
        auto num = i->asNumber();
        if (num == std::nullopt)
            throw LispError("Cannot add a non-numeric value.");
        total += *num;
    }
    return std::make_shared<NumericValue>(total);
}

void Builtins::initSymbolList() {
    EvalEnv::symbol_list["+"] = std::make_shared<BuiltinProcValue>(&add);
}
