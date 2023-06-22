#ifndef BOOST_H
#define BOOST_H

#include <string>
#include "./value.h"

ValuePtr readParse(std::istream&);
void REPLMode();
void fileMode(const std::string&);
[[deprecated]] void interactiveMode(const std::string&, const std::string&);

#endif