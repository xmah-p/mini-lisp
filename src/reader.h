#ifndef READER_H
#define READER_H

#include <deque>
#include <string>

#include "./token.h"

class Reader {
    std::istream& is;
    std::size_t* line_num_ptr;  // for error track in FILEMODE
    const bool FILEMODE;

    // check if expr contains only comments and spaces
    bool emptyExpr(const std::string&);

    // return a string with StringValue and comments removed.
    // also check if quotes match.
    std::string handleInput(std::string);

    // return 0 if parentheses already match, or num of spaces for indentation
    // if not.
    std::size_t notWholeExpr(const std::string&);

public:
    Reader(std::istream& is, std::size_t* line_num_ptr = nullptr)
        : is{is},
          line_num_ptr{line_num_ptr},
          FILEMODE{line_num_ptr ? true : false} {}

    // read a whole expr from is.
    std::string read();

    bool fail();
};

#endif