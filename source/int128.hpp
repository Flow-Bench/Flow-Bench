#pragma once

// 128-bit integer
// for IPv6 address etc.

#include "integer.hpp"
#include "int32.hpp"

namespace flowbench {

class Int128 : public Integer {
private:
    uint64_t high;
    uint64_t low;

public:
    Int128() : high(0), low(0) {}
    Int128(uint64_t low) : high(0), low(low) {}
    Int128(uint64_t high, uint64_t low) : high(high), low(low) {}
    Int128(const Int128& other) = default;
    Int128(Int128&& other) = default;
    Int128& operator=(const Int128& other) = default;
    Int128& operator=(Int128&& other) = default;

    std::unique_ptr<Integer> clone() const override {
        return std::make_unique<Int128>(*this);
    }

    // convert Int32 to Int128
    Int128(const Int32& other) : high(static_cast<uint64_t>(other.getValue()) << 32), low(0) {}
    Int128& operator=(const Int32& other) {
        high = static_cast<uint64_t>(other.getValue()) << 32;
        low = 0;
        return *this;
    }

    bool operator==(const Int128& other) const {
        return high == other.high && low == other.low;
    }

    bool operator!=(const Int128& other) const {
        return high != other.high || low != other.low;
    }

    bool operator<(const Int128& other) const {
        return high < other.high || (high == other.high && low < other.low);
    }

    bool operator<=(const Int128& other) const {
        return high < other.high || (high == other.high && low <= other.low);
    }

    bool operator>(const Int128& other) const {
        return high > other.high || (high == other.high && low > other.low);
    }

    bool operator>=(const Int128& other) const {
        return high > other.high || (high == other.high && low >= other.low);
    }

    Int128 operator~() const {
        return Int128(~high, ~low);
    }

    Int128 operator&(const Int128& other) const {
        return Int128(high & other.high, low & other.low);
    }

    Int128 operator|(const Int128& other) const {
        return Int128(high | other.high, low | other.low);
    }

    Int128 operator^(const Int128& other) const {
        return Int128(high ^ other.high, low ^ other.low);
    }

    // different from actual 128-bit integer multiplication
    // because we only use this function to parse hex string
    Int128 operator*(uint32_t value) const {
        return Int128(high * value, low * value);
    }

    Int128& operator&=(const Int128& other) {
        high &= other.high;
        low &= other.low;
        return *this;
    }

    Int128& operator|=(const Int128& other) {
        high |= other.high;
        low |= other.low;
        return *this;
    }

    Int128& operator^=(const Int128& other) {
        high ^= other.high;
        low ^= other.low;
        return *this;
    }

    Int128 operator<<(uint8_t shift) const {
        if (shift == 0) {
            return *this;
        } else if (shift < 64) {
            return Int128(high << shift, (high >> (64 - shift)) | (low << shift));
        } else if (shift == 64) {
            return Int128(low, 0);
        } else {
            return Int128(low << (shift - 64), 0);
        }
    }

    Int128 operator>>(uint8_t shift) const {
        if (shift == 0) {
            return *this;
        } else if (shift < 64) {
            return Int128((high << (64 - shift)) | (low >> shift), low >> shift);
        } else if (shift == 64) {
            return Int128(0, high);
        } else {
            return Int128(0, high >> (shift - 64));
        }
    }

    Int128& operator<<=(uint8_t shift) {
        if (shift == 0) {
            return *this;
        } else if (shift < 64) {
            high <<= shift;
            high |= low >> (64 - shift);
            low <<= shift;
        } else if (shift == 64) {
            high = low;
            low = 0;
        } else {
            high = low << (shift - 64);
            low = 0;
        }
        return *this;
    }

    Int128& operator>>=(uint8_t shift) {
        if (shift == 0) {
            return *this;
        } else if (shift < 64) {
            low >>= shift;
            low |= high << (64 - shift);
            high >>= shift;
        } else if (shift == 64) {
            low = high;
            high = 0;
        } else {
            low = high >> (shift - 64);
            high = 0;
        }
        return *this;
    }

    bool isZero() const {
        return high == 0 && low == 0;
    }

    bool isMax() const {
        return high == UINT64_MAX && low == UINT64_MAX;
    }

private:
    auto getTrueValues(uint8_t width) const {
        uint64_t trueValueHigh, trueValueLow;
        if (width <= 64) {
            trueValueLow = high >> (64 - width);
            trueValueHigh = 0;
        } else {
            trueValueLow = low >> (128 - width) | (high << (width - 64));
            trueValueHigh = high >> (128 - width);
        }
        return std::make_pair(trueValueHigh, trueValueLow);
    }

public:
    // only high bits are used
    std::string toBinaryString(uint8_t width) const override {
        if (width == 0) {
            return "*";
        } else {
            auto trueValues = getTrueValues(width);
            uint64_t trueValueHigh = trueValues.first;
            uint64_t trueValueLow = trueValues.second;
            std::string result;
            result.resize(width);
            for (uint8_t i = 0; i < width; i++) {
                if (i < 64) {
                    result[width - 1 - i] = (trueValueLow & 1) ? '1' : '0';
                    trueValueLow >>= 1;
                } else {
                    result[width - 1 - i] = (trueValueHigh & 1) ? '1' : '0';
                    trueValueHigh >>= 1;
                }
            }
            return result;
        }
    }

    // only high bits are used
    // we do not support decimal string for Int128
    std::string toDecimalString(uint8_t width) const override {
        auto trueValues = getTrueValues(width);
        uint64_t trueValueHigh = trueValues.first;
        uint64_t trueValueLow = trueValues.second;
        return std::to_string(trueValueHigh) + "\'" + std::to_string(trueValueLow);
    }

    // only high bits are used, no 0x prefix
    std::string toHexString(uint8_t width) const override {
        auto trueValues = getTrueValues(width);
        uint64_t trueValueHigh = trueValues.first;
        uint64_t trueValueLow = trueValues.second;
        std::string result;
        uint8_t digitCount = (width + 3) / 4;
        result.resize(digitCount);
        for (int i = 0; i < digitCount; i++) {
            if (i < 16) {
                result[digitCount - 1 - i] = "0123456789abcdef"[trueValueLow & 0xf];
                trueValueLow >>= 4;
            } else {
                result[digitCount - 1 - i] = "0123456789abcdef"[trueValueHigh & 0xf];
                trueValueHigh >>= 4;
            }
        }
        return result;
    }

};

template <>
uint8_t getBitCount<Int128>() {
    return 128;
}

template <>
const Int128& getMaxOf<Int128>() {
    static const Int128 max = Int128(UINT64_MAX, UINT64_MAX);
    return max;
}

template <>
const Int128& getHighestBitOf<Int128>() {
    static const Int128 highestBit = Int128(1ULL << 63, 0);
    return highestBit;
}

}