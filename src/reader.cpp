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
        if (nextpos == npos) throw SyntaxError("Unmatched quote");
        int len = nextpos - pos + 1;
        str.replace(pos, len, std::string(len, '#'));
    }
    return str;
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
                throw SyntaxError("Unmatched parentheses");
            indent_info.pop();
        }
    }
}

bool Reader::handleComments(std::string& str) {
    auto npos = std::string::npos;
    auto cmt_bg = str.find("#|");
    auto cmt_ed = str.find("|#");

    while (cmt_bg != npos) {
        if (cmt_ed == npos)
            return false;
        else if (cmt_ed < cmt_bg)
            throw SyntaxError("Unmatched comment");
        else
            str.erase(cmt_bg, cmt_ed - cmt_bg + 2);
        cmt_bg = str.find("#|");
        cmt_ed = str.find("|#");
    }
    if (str.find("|#") != npos) throw SyntaxError("Unmatched comment");

    auto pos = str.find(';');
    if (pos != npos) str.erase(pos);
    return true;
}

bool Reader::emptyExpr(const std::string& str) {
    return std::all_of(str.begin(), str.end(),
                       [](char c) { return std::isspace(c); });
}

std::string Reader::read() {
    std::string line;
    bool in_comment = false;
    auto updateFromInput = [&in_comment, this](std::string& line) {
        std::string tmpln;
        std::getline(is, tmpln);
        line = in_comment ? line + " " + tmpln : tmpln;
    };

    do {
        if (FILEMODE)
            (*line_num_ptr)++;
        else
            std::cout << (in_comment ? "... " : ">>> ");
        updateFromInput(line);
        if (is.fail())
            return in_comment ? throw SyntaxError("Unclosed comments") : "";
    } while ((in_comment = !handleComments(line)) || emptyExpr(line));

    std::string currln = line;
    handleIndent(currln);
    while (!indent_info.empty()) {
        do {
            if (FILEMODE)
                (*line_num_ptr)++;
            else
                std::cout << "... " + std::string(indent_info.top(), ' ');
            updateFromInput(currln);
            if (is.fail()) return "";
        } while ((in_comment = !handleComments(currln)) || emptyExpr(currln));

        handleIndent(currln);
        line += " " + currln;
    }
    return line;
}