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

    template <typename T> // T is a container, vector or array
    uint32_t select(T weights) const;
};

template <typename T>
uint32_t RandomSelector::select(T weights) const {
    double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
    if (sum == 0) {
        throw NoCandidateError();
    }
    double r = Random::getInstance().nextDouble(0, sum);
    for (uint32_t i = 0; i < weights.size(); i++) {
        r -= weights[i];
        if (r <= 0) {
            return i;
        }
    }
    return weights.size() - 1;
}

}

