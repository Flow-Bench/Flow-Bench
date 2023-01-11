#pragma once

// Pareto allocator
// we will allocate n traces to some flows according to the Pareto distribution

#include <algorithm>
#include <vector>
#include <numeric>

#include "random.hpp"
#include "pareto_distribution.hpp"

namespace flowbench {

class ParetoAllocator : public Singleton<ParetoAllocator> {
public:
    ParetoAllocator() = default;

    std::vector<uint32_t> allocate(uint32_t traceCount, uint32_t groupCount, const ParetoDistribution& distribution) const;
};

std::vector<uint32_t> ParetoAllocator::allocate(uint32_t traceCount, uint32_t groupCount, const ParetoDistribution& distribution) const {
    std::vector<uint32_t> result;
    uint32_t remain = traceCount;
    while (remain > 0) {
        uint32_t count = distribution.copyCount();
        if (count > remain) {
            count = remain;
        }
        result.push_back(count);
        remain -= count;
        if (result.size() == groupCount) {
            break;
        }
    }
    if (remain > 0) {
        // we need to allocate the remain traces to the groups
        std::vector<double> weights;
        weights.resize(result.size());
        for (uint32_t i = 0; i < result.size(); i++) {
            weights[i] = result[i];
        }
        double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
        for (uint32_t i = 1; i < weights.size(); i++) {
            weights[i] += weights[i - 1];
        }
        for (uint32_t i = 0; i < weights.size(); i++) {
            weights[i] /= sum;
        }
        weights[weights.size() - 1] = 1.0;
        while (remain > 0) {
            double r = Random::getInstance().nextDouble(0, 1);
            uint32_t index = std::lower_bound(weights.begin(), weights.end(), r) - weights.begin();
            result[index]++;
            remain--;
        }
    }
    return result;
}

}