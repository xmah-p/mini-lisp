#ifndef EVAL_ENV_H
#define EVAL_ENV_H


#include "./value.h"

class EvalEnv : public std::enable_shared_from_this<EvalEnv> {
private:
    EvalEnv() = default;
    EvalEnv(std::shared_ptr<EvalEnv> parent) : parent{parent} {}
    EvalEnv(const EvalEnv& env) : parent{env.parent}, symbol_list(env.symbol_list) {}

public:
    std::shared_ptr<EvalEnv> parent{nullptr};
    std::unordered_map<std::string, ValuePtr> symbol_list;

    static std::shared_ptr<EvalEnv> createGlobal();
    std::shared_ptr<EvalEnv> createChild(const std::vector<std::string>& params,
                                         const std::vector<ValuePtr>& args);

    ValuePtr eval(ValuePtr expr);
    std::vector<ValuePtr> evalList(ValuePtr ls);
    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr> args);
    void defineBinding(ValuePtr name, ValuePtr val);
    ValuePtr& lookupBinding(std::string name);
    ValuePtr& lookupBinding(ValuePtr sym);
    std::vector<ValuePtr> getAllTestsName();
};

#endif