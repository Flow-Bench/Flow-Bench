#pragma once

// 64-bit integer
// for MAC address etc.

#include "integer.hpp"
#include "int32.hpp"

namespace flowbench {

class Int64 : public Integer {
private:
    uint64_t value;

public:
    Int64() : value(0) {}
    Int64(uint64_t value) : value(value) {}
    Int64(const Int64& other) = default;
    Int64(Int64&& other) = default;
    Int64& operator=(const Int64& other) = default;
    Int64& operator=(Int64&& other) = default;

    std::unique_ptr<Integer> clone() const override {
        return std::make_unique<Int64>(*this);
    }

    // convert Int32 to Int64
    Int64(const Int32& other) : value(static_cast<uint64_t>(other.getValue()) << 32) {}
    Int64& operator=(const Int32& other) {
        value = static_cast<uint64_t>(other.getValue()) << 32;
        return *this;
    }

    bool operator==(const Int64& other) const {
        return value == other.value;
    }

    bool operator!=(const Int64& other) const {
        return value != other.value;
    }

    bool operator<(const Int64& other) const {
        return value < other.value;
    }

    bool operator<=(const Int64& other) const {
        return value <= other.value;
    }

    bool operator>(const Int64& other) const {
        return value > other.value;
    }

    bool operator>=(const Int64& other) const {
        return value >= other.value;
    }

    Int64 operator~() const {
        return Int64(~value);
    }

    Int64 operator&(const Int64& other) const {
        return Int64(value & other.value);
    }

    Int64 operator|(const Int64& other) const {
        return Int64(value | other.value);
    }

    Int64 operator^(const Int64& other) const {
        return Int64(value ^ other.value);
    }

    Int64 operator*(uint32_t value) const {
        return Int64(this->value * value);
    }

    Int64& operator&=(const Int64& other) {
        value &= other.value;
        return *this;
    }

    Int64& operator|=(const Int64& other) {
        value |= other.value;
        return *this;
    }

    Int64& operator^=(const Int64& other) {
        value ^= other.value;
        return *this;
    }

    Int64 operator<<(uint8_t shift) const {
        return Int64(value << shift);
    }

    Int64 operator>>(uint8_t shift) const {
        return Int64(value >> shift);
    }

    Int64& operator<<=(uint8_t shift) {
        value <<= shift;
        return *this;
    }

    Int64& operator>>=(uint8_t shift) {
        value >>= shift;
        return *this;
    }

    bool isZero() const {
        return value == 0;
    }

    bool isMax() const {
        return value == UINT64_MAX;
    }

    // only high bits are used
    std::string toBinaryString(uint8_t width) const override {
        if (width == 0) {
            return "*";
        } else {
            uint64_t trueValue = value >> (64 - width);
            std::string result;
            result.resize(width);
            for (int i = 0; i < width; i++) {
                result[width - i - 1] = (trueValue & 1) ? '1' : '0';
                trueValue >>= 1;
            }
            return result;
        }
    }

    // only high bits are used
    std::string toDecimalString(uint8_t width) const override {
        uint64_t trueValue = value >> (64 - width);
        return std::to_string(trueValue);
    }

    // only high bits are used, no 0x prefix
    std::string toHexString(uint8_t width) const override {
        uint64_t trueValue = value >> (64 - width);
        std::string result;
        uint8_t digitCount = (width + 3) / 4;
        result.resize(digitCount);
        for (int i = 0; i < digitCount; i++) {
            result[digitCount - i - 1] = "0123456789abcdef"[trueValue & 0xf];
            trueValue >>= 4;
        }
        return result;
    }
};

template <>
uint8_t getBitCount<Int64>() {
    return 64;
}

template <>
const Int64& getMaxOf<Int64>() {
    static const Int64 max(UINT64_MAX);
    return max;
}

template <>
const Int64& getHighestBitOf<Int64>() {
    static const Int64 highestBit(1ULL << 63);
    return highestBit;
}

}