#pragma once

// define a MT19337 random engine

#include <cstdint>

#include "singleton.hpp"

namespace flowbench {

class RandomEngine : public Singleton<RandomEngine> {
private:
    constexpr uint32_t temper(uint32_t y) const {
        y ^= y >> 11;
        y ^= y << 7 & 0x9d2c5680;
        y ^= y << 15 & 0xefc60000;
        y ^= y >> 18;
        return y;
    }

private:
    uint32_t m_mt[624];
    uint32_t m_index;
    void twist() {
        for (int i = 0; i < 624; i++) {
            uint32_t y = (m_mt[i] & 0x80000000) + (m_mt[(i + 1) % 624] & 0x7fffffff);
            m_mt[i] = m_mt[(i + 397) % 624] ^ y >> 1;
            if (y % 2 != 0) {
                m_mt[i] ^= 0x9908b0df;
            }
        }
        m_index = 0;
    }

public:
    RandomEngine() : m_index(624) {}

    void srand(uint32_t seed);
    uint32_t rand();
};

void RandomEngine::srand(uint32_t seed) {
    m_mt[0] = seed;
    for (int i = 1; i < 624; i++) {
        m_mt[i] = 0x6c078965 * (m_mt[i - 1] ^ m_mt[i - 1] >> 30) + i;
    }
    m_index = 624;
}

uint32_t RandomEngine::rand() {
    if (m_index >= 624) {
        twist();
    }
    uint32_t y = temper(m_mt[m_index]);
    m_index++;
    return y;
}

}