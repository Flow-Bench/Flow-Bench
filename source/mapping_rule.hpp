#pragma once

// the third step of the trace generation
// in the first step, we have splitted the rule set into isolate rules
// in the second step, we have allocated the traces according to the Pareto distribution
// in this step, we will build a mapping between the allocation result and the isolate rule set

#include <algorithm>
#include <numeric>

#include "exception.hpp"
#include "rule_set_ud_index.hpp"

namespace flowbench {

class RuleMapping : public Singleton<RuleMapping> {
private:
    // share the same index with RuleAllocator's ruleFlowAllocation
    UDRuleSetWithIndex ruleSet;

    std::vector<bool> visited;
    uint32_t findNearestUnvisited(uint32_t index, uint32_t lowerBound) const;

public:
    RuleMapping() = default;

    UDRuleSetWithIndex& operator()(UDRuleSet& isolateRuleSet, std::vector<std::vector<uint32_t>>& ruleFlowAllocation);
};

uint32_t RuleMapping::findNearestUnvisited(uint32_t index, uint32_t lowerBound) const {
    uint32_t r = index;
    while (visited[r] && r < visited.size()) {
        r++;
    }
    if (r == visited.size()) {
        r = index;
        while (r >= lowerBound && r < visited.size() && visited[r]) {
            r--;
        }
    }
    return r;
}

UDRuleSetWithIndex& RuleMapping::operator()(UDRuleSet& isolateRuleSet, std::vector<std::vector<uint32_t>>& ruleFlowAllocation) {
    ruleSet.clear();
    visited.clear();
    visited.resize(isolateRuleSet.size());
    std::fill(visited.begin(), visited.end(), false);
    for (auto& allocation : ruleFlowAllocation) {
        if (allocation.empty()) {
            continue;
        }
        double log2FlowCount = std::log2(allocation.size());
        // isolateRuleSet is sorted by available width
        // find the first rule whose available width is larger than log2FlowCount
        // then the rule before it is the largest rule that can hold the allocation
        uint32_t lowerBound = std::lower_bound(isolateRuleSet.begin(), isolateRuleSet.end(), log2FlowCount,
                                               [](const std::unique_ptr<UDRule>& rule, double value) {
                                                   return rule == nullptr || rule->getAvailableWidth() < value;
                                               }) - isolateRuleSet.begin();
        uint32_t ruleIndex;
        if (lowerBound < isolateRuleSet.size()) {
            ruleIndex = Random::getInstance().nextInt32(lowerBound, isolateRuleSet.size() - 1);
            ruleIndex = findNearestUnvisited(ruleIndex, lowerBound);
        }
        if (lowerBound == isolateRuleSet.size() || ruleIndex < lowerBound || ruleIndex >= isolateRuleSet.size()) {
            // no rule can hold the allocation, merge the allocation into one
            uint32_t sum = std::accumulate(allocation.begin(), allocation.end(), 0);
            allocation.clear();
            allocation.push_back(sum);
            ruleIndex = Random::getInstance().nextInt32(0, isolateRuleSet.size() - 1);
            ruleIndex = findNearestUnvisited(ruleIndex, 0);
        }
        if (ruleIndex >= isolateRuleSet.size()) {
            // still no rule can hold the allocation, error
            throw NoRuleError();
        }
        visited[ruleIndex] = true;
        ruleSet.push_back(std::move(isolateRuleSet[ruleIndex]), ruleIndex);
    }
    return ruleSet;
}

}