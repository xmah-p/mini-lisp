#ifndef READER_H
#define READER_H

#include <stack>
#include <string>

#include "./token.h"

class Reader {
    std::istream& is;
    std::size_t* line_num_ptr;  // for error track in FILEMODE
    const bool FILEMODE;
    std::stack<std::size_t> indent_info;

    // check if expr contains only comments and spaces
    bool emptyExpr(const std::string&);

    // return a string with StringValue and comments muted.
    // also check if quotes match.
    std::string handleInput(std::string);

    // update indent_info
    void handleIndent(const std::string&);

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