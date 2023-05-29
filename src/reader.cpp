#include "./reader.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>

#include "./error.h"

bool Reader::fail() {
    return is.fail();
}

std::string Reader::muteString(std::string str) {
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

void Reader::removeComments(std::string& str) {
    auto npos = std::string::npos;
    auto cmt_bg = str.find("#|");
    auto cmt_ed = str.find("|#");

    while (cmt_bg != npos) {
        if (cmt_ed == npos || cmt_ed < cmt_bg)
            throw SyntaxError("Unmatched comment!");
        else
            str.erase(cmt_bg, cmt_ed - cmt_bg + 2);
        cmt_bg = str.find("#|");
        cmt_ed = str.find("|#");
    }
    if (str.find("|#") != npos) throw SyntaxError("Unmatched comment!");
    auto pos = str.find(';');
    if (pos != npos) str.erase(pos);
}

void Reader::handleIndent(const std::string& expr) {
    std::string str = muteString(expr);
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
    auto npos = std::string::npos;
    auto cmt_bg = str.find("#|");
    auto cmt_ed = str.find("|#");

    if (!in_comment && cmt_ed < cmt_bg) throw SyntaxError("Unmatched comment!");
    if (cmt_ed != npos) in_comment = false;
    if (in_comment || str.empty()) return true;
    if (cmt_bg != npos) {
        if (cmt_ed == npos || cmt_ed < cmt_bg)
            in_comment = true;
        else
            return emptyExpr(str.substr(0, cmt_bg) + str.substr(cmt_ed + 2));
    }

    auto end = std::min(str.length(), str.find(';'));
    end = std::min(end, cmt_bg);
    auto bg = std::min(str.length(), cmt_ed + 2);

    auto is_space = [](char c) { return std::isspace(c); };
    if (std::all_of(str.begin(), str.begin() + end, is_space) &&
        std::all_of(str.begin() + bg, str.end(), is_space))
        return true;
    return false;
}

std::string Reader::read() {
    std::string line;
    do {
        if (FILEMODE)
            (*line_num_ptr)++;
        else
            std::cout << (in_comment ? "... " : ">>> ");
        std::string tmpln;
        std::getline(is, tmpln);
        line = in_comment ? line + " " + tmpln : tmpln;
        if (is.fail())
            return in_comment ? throw SyntaxError("Unclosed comments!") : "";
    } while (emptyExpr(line) || in_comment);
    removeComments(line);
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
            std::string tmpln;
            std::getline(is, tmpln);
            currln = in_comment ? currln + " " + tmpln : tmpln;
            if (is.fail()) return "";
        } while (emptyExpr(currln) || in_comment);

        removeComments(currln);
        handleIndent(currln);
        line += " " + currln;
    }
    return line;
}