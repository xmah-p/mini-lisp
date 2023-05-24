#include "./reader.h"

#include <iostream>
#include <numeric>
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
        int len = nextpos - pos + 1;
        str.replace(pos, len, std::string(len, '#'));
    }
    return str;
}

void Reader::handleIndent(const std::string& expr) {
    std::string str = handleInput(expr);
    bool first_Lparen = true;

    int prev = 0;
    for (std::size_t i = 0; i != str.length(); ++i) {
        char c = str[i];
        if (c == '(') {
            std::size_t pos = i + 1;
            while (pos < str.length() && std::isspace(str[pos])) ++pos;
            while (pos < str.length() && !std::isspace(str[pos])) ++pos;
            if (first_Lparen && !indent_info.empty()) prev = indent_info.top();
            indent_info.push(prev + pos + 1);
            first_Lparen = false;
        } else if (c == ')') {
            if (indent_info.empty())
                throw SyntaxError("Unmatched parentheses!");
            indent_info.pop();
        }
    }
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

    std::string currln = line;
    handleIndent(currln);
    while (!indent_info.empty()) {
        auto str_repeat = [](std::string str, std::size_t n) {
            std::string res;
            for (std::size_t i = 0; i != n; ++i) res += str;
            return res;
        };

        do {
            if (FILEMODE)
                (*line_num_ptr)++;
            else
                std::cout << "... " + str_repeat(" ", indent_info.top());
            std::getline(is, currln);
            if (is.fail()) return "";
        } while (emptyExpr(currln));

        handleIndent(currln);
        line += " " + currln;
    }
    return line;
}