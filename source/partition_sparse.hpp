#pragma once

// the sparse partition algorithm
// it works in case of very sparse rules

#include <queue>

#include "parameter_calculator.hpp"
#include "partition.hpp"
#include "singleton.hpp"
#include "problem_state.hpp"
#include "rule_splitter.hpp"

namespace flowbench {

class SparsePartition : public Partition , public Singleton<SparsePartition> {
public:
    SparsePartition(): Partition() {}

    // we will partition the problem into p sub-problems
    // at the beginning, p = 1
    // if we find that the bit width is insufficient, we will increase p (multiply 2)
    // if p is too large (exceed n / exceed 2^w), return false
    // if p makes the DAG too sparse (parameter too small), return false
    // otherwise, return true
    bool addPartition();

    // export the origins of the sub-problems
    // if the partition is not finished, return false
    // otherwise, return true
    bool exportOrigins(UDRuleSet& finalSet, std::queue<std::unique_ptr<ProblemState>> &origins) const override;
};

bool SparsePartition::addPartition() {
    partCount *= 2;
    if (partCount > n || std::log2(partCount) > totalWidth) {
        return false;
    }
    uint32_t smallPart = n / partCount;
    uint32_t largePart = smallPart + 1;
    uint32_t largeCount = n % partCount;
    uint32_t smallCount = partCount - largeCount;
    uint32_t mp = smallCount * ParameterCalculator::getInstance().at(smallPart) + largeCount * ParameterCalculator::getInstance().at(largePart);
    return mp >= p;
}

bool SparsePartition::exportOrigins(UDRuleSet& finalSet, std::queue<std::unique_ptr<ProblemState>>& origins) const {
    std::queue<std::unique_ptr<UDRule>> Q;
    Q.push(std::make_unique<UDRule>());
    while (Q.size() < partCount && !Q.empty()) {
        auto rule = std::move(Q.front());
        Q.pop();
        auto pair = RuleSplitter::getInstance().split(*rule);
        if (pair.first != nullptr) {
            Q.push(std::move(pair.first));
        }
        if (pair.second != nullptr) {
            Q.push(std::move(pair.second));
        }
    }
    if (Q.size() != partCount) {
        return false;
    }
    uint32_t smallPart = n / partCount;
    uint32_t largePart = smallPart + 1;
    uint32_t largeCount = n % partCount;
    uint32_t smallCount = partCount - largeCount;
    uint32_t smallParameter = std::min(ParameterCalculator::getInstance().at(smallPart), p / smallCount);
    uint32_t sumOfLargeParameters = p - smallParameter * smallCount;
    for (uint32_t i = 0; i < smallCount; i++) {
        origins.push(std::make_unique<ProblemState>(smallPart, smallParameter, true, std::move(Q.front())));
        Q.pop();
    }
    for (uint32_t i = 0; i < largeCount; i++) {
        uint32_t largeParameter = std::min(ParameterCalculator::getInstance().at(largePart), sumOfLargeParameters / (largeCount - i));
        origins.push(std::make_unique<ProblemState>(largePart, largeParameter, true, std::move(Q.front())));
        Q.pop();
        sumOfLargeParameters -= largeParameter;
    }
    return sumOfLargeParameters == 0;
}

}