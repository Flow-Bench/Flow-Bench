#pragma once

// 32-bit integer
// for IPv4 address etc.
// RM only support 32-bit integer

#include "integer.hpp"

namespace flowbench {

class Int32 : public Integer {
private:
    uint32_t value;

public:
    Int32() : value(0) {}
    Int32(uint32_t value) : value(value) {}
    Int32(const Int32& other) = default;
    Int32(Int32&& other) = default;
    Int32& operator=(const Int32& other) = default;
    Int32& operator=(Int32&& other) = default;

    std::unique_ptr<Integer> clone() const override {
        return std::make_unique<Int32>(*this);
    }

    bool operator==(const Int32& other) const {
        return value == other.value;
    }

    bool operator!=(const Int32& other) const {
        return value != other.value;
    }

    bool operator<(const Int32& other) const {
        return value < other.value;
    }

    bool operator<=(const Int32& other) const {
        return value <= other.value;
    }

    bool operator>(const Int32& other) const {
        return value > other.value;
    }

    bool operator>=(const Int32& other) const {
        return value >= other.value;
    }

    Int32 operator~() const {
        return Int32(~value);
    }

    Int32 operator&(const Int32& other) const {
        return Int32(value & other.value);
    }

    Int32 operator|(const Int32& other) const {
        return Int32(value | other.value);
    }

    Int32 operator^(const Int32& other) const {
        return Int32(value ^ other.value);
    }

    Int32 operator*(uint32_t value) const {
        return Int32(this->value * value);
    }

    Int32& operator&=(const Int32& other) {
        value &= other.value;
        return *this;
    }

    Int32& operator|=(const Int32& other) {
        value |= other.value;
        return *this;
    }

    Int32& operator^=(const Int32& other) {
        value ^= other.value;
        return *this;
    }

    Int32 operator<<(uint8_t shift) const {
        return Int32(value << shift);
    }

    Int32 operator>>(uint8_t shift) const {
        return Int32(value >> shift);
    }

    Int32& operator<<=(uint8_t shift) {
        value <<= shift;
        return *this;
    }

    Int32& operator>>=(uint8_t shift) {
        value >>= shift;
        return *this;
    }

    bool isZero() const override {
        return value == 0;
    }

    bool isMax() const override {
        return value == UINT32_MAX;
    }

    uint32_t getValue() const override {
        return value;
    }

    // only high bits are used
    std::string toBinaryString(uint8_t width) const override {
        if (width == 0) {
            return "*";
        } else {
            uint32_t trueValue = value >> (32 - width);
            std::string result;
            result.resize(width);
            for (uint8_t i = 0; i < width; i++) {
                result[width - i - 1] = (trueValue & 1) ? '1' : '0';
                trueValue >>= 1;
            }
            return result;
        }
    }

    // only high bits are used
    std::string toDecimalString(uint8_t width) const override {
        uint32_t trueValue = value >> (32 - width);
        return std::to_string(trueValue);
    }

    // only high bits are used, no 0x prefix
    std::string toHexString(uint8_t width) const override {
        uint32_t trueValue = value >> (32 - width);
        std::string result;
        uint8_t digitCount = (width + 3) / 4;
        result.resize(digitCount);
        for (uint8_t i = 0; i < digitCount; i++) {
            result[digitCount - i - 1] = "0123456789abcdef"[trueValue & 0xf];
            trueValue >>= 4;
        }
        return result;
    }
};

}