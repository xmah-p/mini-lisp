#include "./reader.h"

#include <iostream>
#include <stack>
#include <string>

#include "./error.h"
#include "./tokenizer.h"

std::size_t Reader::notWholeExpr(const std::string& str) {
    std::stack<std::size_t> stk;
    for (std::size_t i = 0; i != str.length(); ++i) {
        char c = str[i];
        if (c == '(')
            stk.push(i);
        else if (c == ')') {
            if (stk.empty()) throw SyntaxError("Unmatched parentheses!");
            stk.pop();
        }
    }
    if (stk.empty()) return 0;
    std::size_t indent = stk.top() + 1;
    while (std::isspace(str[indent])) ++indent;
    while (!std::isspace(str[indent])) ++indent;
    return indent + 2;
}

bool Reader::emptyExpr(const std::string& str) {
    if (str == "" || str[0] == ';') return true;
    if (std::all_of(str.begin(), str.end(),
                    [](char c) { return std::isspace(c); }))
        return true;
    return false;
}

std::deque<TokenPtr> Reader::read() {
    std::string line;
    do {
        if (FILEMODE)
            (*line_num_ptr)++;
        else
            std::cout << ">>> ";
        std::getline(is, line);
        if (is.fail()) return {};
    } while (emptyExpr(line));

    while (std::size_t n = notWholeExpr(line)) {
        auto str_repeat = [](std::string str, std::size_t n) {
            std::string res;
            for (std::size_t i = 0; i != n; ++i) res += str;
            return res;
        };
        std::string nextln;
        do {
            if (FILEMODE)
                (*line_num_ptr)++;
            else
                std::cout << "..." + str_repeat(" ", n);
            std::getline(is, nextln);
            if (is.fail()) return {};
        } while (emptyExpr(nextln));

        line += " " + nextln;
    }
    auto tokens = Tokenizer::tokenize(line);
    return tokens;
}

std::deque<TokenPtr> Reader::read(std::istream& is, std::size_t* ptr) {
    return Reader(is, ptr).read();
}