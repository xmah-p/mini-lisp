#ifndef READER_H
#define READER_H

#include <string>

class Reader {

public:
    static std::string readFrom(std::istream_iterator<char>);

};

#endif