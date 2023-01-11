#pragma once

// a integer base class
// support 32-128 bit integer

#include <cstdint>
#include <ostream>
#include <string>

namespace flowbench {

class Integer {
public:
    Integer() {}
    Integer(uint32_t) {}

    virtual bool isZero() const = 0;
    virtual bool isMax() const = 0;

    // requires:
    // ==, !=, <, >, <=, >=
    // ~, &, |, ^, &=, |=, ^=
    // <<, >>, <<=, >>=

    virtual std::unique_ptr<Integer> clone() const = 0;

public:
    // for output format, only high bits are used
    virtual std::string toBinaryString(uint8_t width) const = 0;  // LPM
    virtual std::string toDecimalString(uint8_t width) const = 0; // RM
    virtual std::string toHexString(uint8_t width) const = 0;     // EM
};

template <class T> // where T : Integer
uint8_t getBitCount() {
    return 32;
}

template <class T> // where T : Integer
const T& getZeroOf() {
    const static T zero;
    return zero;
}

template <class T> // where T : Integer
const T& getMaxOf() {
    const static T max(0xFFFFFFFF);
    return max;
}

template <class T> // where T : Integer
const T& getHighestBitOf() {
    const static T highestBit(0x80000000);
    return highestBit;
}

}