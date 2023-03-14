#ifndef PARSER_H
#define PARSER_H

#include <deque>

#include "./token.h"
#include "./value.h"

class Parser {
private:
    std::deque<TokenPtr> tokens;

public:
    Parser() = default;
    Parser(std::deque<TokenPtr> tokens) : tokens{std::move(tokens)} {}
    ValuePtr parse();
};

#endif