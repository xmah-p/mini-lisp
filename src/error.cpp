#include "./error.h"

#include <iostream>

void SyntaxError::handle() {
    std::cerr << "SyntaxError: " << what() << std::endl;
}

void LispError::handle() {
    std::cerr << "LispError: " << what() << std::endl;
}

void TypeError::handle() {
    std::cerr << "TypeError: " << what() << std::endl;
}

void TestFailure::handle() {}