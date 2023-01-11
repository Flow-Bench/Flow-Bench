#pragma once

// a random number generator
// based on our random engine (MT19937)

#include "random_engine.hpp"
#include "int32.hpp"
#include "int64.hpp"
#include "int128.hpp"


namespace flowbench {

class Random : public Singleton<Random> {
public:
    Random() = default;
    Random(uint32_t seed) {
        RandomEngine::getInstance().srand(seed);
    }

    uint32_t nextUInt32() const {
        return RandomEngine::getInstance().rand();
    }

    int32_t nextInt32(int32_t min, int32_t max) const {
        return min + static_cast<int32_t>(nextUInt32() % (max - min + 1));
    }

    uint32_t nextUInt32(uint32_t min, uint32_t max) const {
        if (min == 0 && max == 4294967295) {
            return nextUInt32();
        }
        return min + nextUInt32() % (max - min + 1);
    }

    double nextDouble(double min, double max) const {
        return min + (max - min) * nextUInt32() / 4294967295.0;
    }

    template <class T> // where T : Integer
    T nextAs() const {
        return T(nextUInt32());
    }

};

template <>
Int64 Random::nextAs<Int64>() const {
    Int64 result = nextUInt32();
    result <<= 32;
    result |= nextUInt32();
    return result;
}

template <>
Int128 Random::nextAs<Int128>() const {
    Int128 result = nextUInt32();
    result <<= 32;
    result |= nextUInt32();
    result <<= 32;
    result |= nextUInt32();
    result <<= 32;
    result |= nextUInt32();
    return result;
}

}