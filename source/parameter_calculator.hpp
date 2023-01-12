#pragma once

// a parameter calculator
// for D and E, we will calculate a baseline value (for relative value = 1.0)
// this value is O(nlogn) (n is the number of rules) because we use a tree-based algorithm to generate the rules
// for every value between 0 and the baseline, FlowBench is able to generate a rule set satisfying the requirement
// for values larger than the baseline, there is risk that FlowBench cannot generate a rule set satisfying the requirement
// we make some efforts to reduce the risk (dense mode, etc.) but too large D or E may still cause the problem
// in that case, we will raise an exception

// we use a recursive algorithm to calculate the parameter
// to avoid repeated calculation, we use a memoization array to store the result
// the total time of the algorithm is O(n)

#include "singleton.hpp"
#include "divider_manager.hpp"

namespace flowbench {

class ParameterCalculator : public Singleton<ParameterCalculator> {
private:
    const static std::array<uint32_t, QD_VERTEX_CNT+1> remainder;

    std::unique_ptr<uint32_t[]> mp;

public:
    ParameterCalculator() = default;
    explicit ParameterCalculator(uint32_t n);
    uint32_t at(uint32_t n);
};

ParameterCalculator::ParameterCalculator(uint32_t n) {
    mp = std::make_unique<uint32_t[]>(std::max<uint32_t>(n, QD_VERTEX_CNT)+1);
    std::fill(mp.get(), mp.get()+n+1, 0);
    std::copy(remainder.begin(), remainder.end(), mp.get());
}

uint32_t ParameterCalculator::at(uint32_t n) {
    if (n <= QD_VERTEX_CNT) {
        return remainder[n];
    }
    if (mp[n] == 0) {
        mp[n] = QD_VERTEX_CNT * (n - QD_VERTEX_CNT) + remainder[QD_VERTEX_CNT];
        const auto& d = DividerManager::getInstance().getDivider(n - QD_VERTEX_CNT);
        // Divider d(n - QD_VERTEX_CNT);
        for (uint8_t i = 0; i < QD_VERTEX_CNT; i++) {
            mp[n] += at(d.result[i]);
        }
    }
    return mp[n];
}

const std::array<uint32_t, QD_VERTEX_CNT+1> ParameterCalculator::remainder =  {
    0, 0, 1, 3, 6
};

}