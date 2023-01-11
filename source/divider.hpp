#pragma once

// this class is used to divide n into 4 parts
// n1 + n2 + n3 + n4 = n

#include <cstdint>
#include <array>
#include <cmath>

#include "constants.hpp"

namespace flowbench {

class Divider {
public:
    std::array<uint32_t, QD_VERTEX_CNT> result;
    explicit Divider(uint32_t n);
};

Divider::Divider(uint32_t n) {
    double cdf = 0.0;
    uint32_t u, v = 0;
    for (uint8_t i = 0; i < QD_VERTEX_CNT; i++) {
        cdf += 1.0 / QD_VERTEX_CNT;
        u = static_cast<uint32_t>(std::round(cdf * n));
        result[i] = u - v;
        v = u;
    }
}

}