#pragma once

// a random selector
// select a random element from a container, vector or array

#include <numeric>

#include "random.hpp"
#include "exception.hpp"

namespace flowbench {

class RandomSelector : public Singleton<RandomSelector> {
public:
    RandomSelector() = default;
    uint32_t select(auto&& weights) const;
};

uint32_t RandomSelector::select(auto&& weights) const {
    auto sum = std::accumulate(weights.begin(), weights.end(), 0);
    if (sum == 0) {
        throw NoCandidateError();
    }
    auto r = Random::getInstance().nextDouble(0, sum);
    for (uint32_t i = 0; i < weights.size(); i++) {
        r -= weights[i];
        if (r <= 0) {
            return i;
        }
    }
    return weights.size() - 1;
}

}

