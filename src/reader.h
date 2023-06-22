#ifndef READER_H
#define READER_H

#include <stack>
#include <string>

class Reader {
    std::istream& is;
    std::size_t* line_num_ptr;  // for error tracking in FILEMODE
    const bool FILEMODE;
    std::stack<std::size_t> indent_info;

    bool emptyExpr(const std::string&);    // whether expr contains only spaces

    std::string muteString(std::string);  // also check if quotes match
    bool handleComments(std::string&);
    void handleIndent(const std::string&);

public:
    Reader(std::istream& is, std::size_t* line_num_ptr = nullptr)
        : is{is},
          line_num_ptr{line_num_ptr},
          FILEMODE{line_num_ptr ? true : false} {}

    std::string read();

    bool fail();
};

#endif