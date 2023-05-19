#include "./reader.h"

#include <iostream>
#include <stack>
#include <string>

#include "./error.h"

bool Reader::fail() {
    return is.fail();
}

std::string Reader::handleInput(std::string str) {
    auto npos = std::string::npos;
    if (str.find(';') != npos) str.erase(str.find(';'));

    for (auto pos = str.find('"'); pos != npos; pos = str.find('"')) {
        auto nextpos = str.find('"', pos + 1);
        if (nextpos == npos) throw SyntaxError("Unmatched quote!");
        str.erase(pos, nextpos - pos + 1);
    }
    return str;
}

std::size_t Reader::notWholeExpr(const std::string& expr) {
    std::string str = handleInput(expr);
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
    while (std::isspace(str[indent]) && indent < str.length()) ++indent;
    while (!std::isspace(str[indent]) && indent < str.length()) ++indent;
    return indent + 2;
}

bool Reader::emptyExpr(const std::string& str) {
    auto pos = std::min(str.length(), str.find(';'));
    if (str == "") return true;
    if (std::all_of(str.begin(), str.begin() + pos,
                    [](char c) { return std::isspace(c); }))
        return true;
    return false;
}

std::string Reader::read() {
    std::string line;
    do {
        if (FILEMODE)
            (*line_num_ptr)++;
        else
            std::cout << ">>> ";
        std::getline(is, line);
        if (is.fail()) return "";
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
            if (is.fail()) return "";
        } while (emptyExpr(nextln));

        line += " " + nextln;
    }
    return line;
}
