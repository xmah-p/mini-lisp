#ifndef ERROR_H
#define ERROR_H

#include <stdexcept>

class Error : public std::runtime_error {
public:
    using runtime_error::runtime_error;
    virtual void handle() = 0;
};

class SyntaxError : public Error {
public:
    using Error::Error;
    void handle() override;
};

class LispError : public Error {
public:
    using Error::Error;
    void handle() override;
};

class TypeError : public Error {
public:
    using Error::Error;
    void handle() override;
};

class TestFailure : public Error {
public:
    using Error::Error;
    void handle() override;
};

#endif