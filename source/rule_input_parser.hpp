#pragma once

// 3 ways to parse strings to integers

#include "rule.hpp"
#include "configuration.hpp"

namespace flowbench {

template <class T> // where T : Integer
T parseBinaryString(const std::string& str) {
    T value = getZeroOf<T>();
    T bit = getHighestBitOf<T>();
    for (uint8_t i = 0; i < str.size(); i++) {
        if (str[i] == '1') {
            value |= bit;
        }
        bit >>= 1;
    }
    return value;
}

template <class T> // where T : Integer
T parseHexString(const std::string& str, uint8_t width) {
    T value = getZeroOf<T>();
    T bit = getHighestBitOf<T>() >> 3;
    // omit "0x"
    for (uint8_t i = 2; i < str.size(); i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            value += bit * (str[i] - '0');
        } else if (str[i] >= 'a' && str[i] <= 'f') {
            value += bit * (str[i] - 'a' + 10);
        } else if (str[i] >= 'A' && str[i] <= 'F') {
            value += bit * (str[i] - 'A' + 10);
        }
        bit >>= 4;
    }
    if (width % 4 != 0) {
        value <<= 4 - (width % 4);
    }
    return value;
}

template <class T> // where T : Integer
T parseDecimalString(const std::string& str, uint8_t width) {
    return getZeroOf<T>();
}

template <>
Int32 parseDecimalString<Int32>(const std::string& str, uint8_t width) {
    uint32_t value = std::stoul(str);
    uint32_t trueValue = value << (32 - width);
    return Int32(trueValue);
}

template <>
Int64 parseDecimalString<Int64>(const std::string& str, uint8_t width) {
    uint64_t value = std::stoull(str);
    uint64_t trueValue = value << (64 - width);
    return Int64(trueValue);
}

template <>
Int128 parseDecimalString<Int128>(const std::string& str, uint8_t width) {
    auto index = str.find('\'');
    if (index == std::string::npos) {
        return getZeroOf<Int128>();
    }
    uint64_t high = std::stoull(str.substr(0, index));
    uint64_t low = std::stoull(str.substr(index + 1));
    uint64_t trueHigh, trueLow;
    if (width <= 64) {
        trueLow = 0;
        trueHigh = low << (64 - width);
    } else {
        trueLow = low << (128 - width);
        trueHigh = high << (128 - width) | (low >> (width - 64));
    }
    return Int128(trueHigh, trueLow);
}

}