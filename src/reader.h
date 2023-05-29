#ifndef READER_H
#define READER_H

#include <stack>
#include <string>

class Reader {
    std::istream& is;
    std::size_t* line_num_ptr;  // for error tracking in FILEMODE
    const bool FILEMODE;
    std::stack<std::size_t> indent_info;
    bool in_comment = false;

    // check if expr contains only comments and spaces
    bool emptyExpr(const std::string&);

    // return a string with StringValues muted also check if quotes match.
    std::string muteString(std::string);
    void removeComments(std::string&);
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