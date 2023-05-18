#ifndef READER_H
#define READER_H

#include <deque>
#include <string>

#include "./token.h"

class Reader {
    std::istream& is;
    std::size_t* line_num_ptr;
    const bool FILEMODE;
    Reader(std::istream& is, std::size_t* line_num_ptr)
        : is{is},
          line_num_ptr{line_num_ptr},
          FILEMODE{line_num_ptr ? true : false} {}

    std::deque<TokenPtr> read();
    bool emptyExpr(const std::string&);
    std::size_t notWholeExpr(
        const std::string&);  // return 0 if whole expr, or num of spaces
                              // for indentation if not whole expr

public:
    static std::deque<TokenPtr> read(std::istream&, std::size_t* ptr = nullptr);
};

#endif