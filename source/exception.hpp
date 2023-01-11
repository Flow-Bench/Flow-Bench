#pragma once

#include <exception>

// error classes

namespace flowbench {

class BitWidthError : public std::exception {
public:
    const char* what() const noexcept override {
        return "Bit width is not sufficient error";
    }
};

class NoCandidateError : public std::exception {
public:
    const char* what() const noexcept override {
        return "No candidate can be selected error";
    }
};

class NoRuleError : public std::exception {
public:
    const char* what() const noexcept override {
        return "No rule can be selected error";
    }
};

}