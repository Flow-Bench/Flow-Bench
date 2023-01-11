#pragma once

// the third step of the trace generation
// in the first step, we have splitted the rule set into isolate rules
// in the second step, we have allocated the traces according to the Pareto distribution
// in this step, we will build a mapping between the allocation result and the isolate rule set

#include <algorithm>
#include <numeric>

#include "exception.hpp"
#include "rule_set.hpp"

namespace flowbench {

class RuleMapping : public Singleton<RuleMapping> {
private:
    // share the same index with RuleAllocator's ruleFlowAllocation
    UDRuleSet ruleSet;

    std::vector<bool> visited;
    uint32_t findNearestUnvisited(uint32_t index) const;

public:
    RuleMapping() = default;

    UDRuleSet& operator()(UDRuleSet& isolateRuleSet, std::vector<std::vector<uint32_t>>& ruleFlowAllocation);
};

uint32_t RuleMapping::findNearestUnvisited(uint32_t index) const {
    uint32_t r = index;
    while (visited[r] && r < visited.size()) {
        r++;
    }
    if (r == visited.size()) {
        r = index;
        while (r < visited.size() && visited[r]) {
            r--;
        }
    }
    return r;
}

UDRuleSet& RuleMapping::operator()(UDRuleSet& isolateRuleSet, std::vector<std::vector<uint32_t>>& ruleFlowAllocation) {
    ruleSet.clear();
    visited.clear();
    visited.resize(isolateRuleSet.size());
    std::fill(visited.begin(), visited.end(), false);
    for (auto& allocation : ruleFlowAllocation) {
        if (allocation.empty()) {
            ruleSet.push_back(nullptr);
        }
        double log2FlowCount = std::log2(allocation.size());
        uint32_t maxRuleIndex = std::lower_bound(isolateRuleSet.begin(), isolateRuleSet.end(), log2FlowCount, [](const std::unique_ptr<UDRule> rule, double log2FlowCount) {
            return rule->getAvailableWidth() < log2FlowCount;
        }) - isolateRuleSet.begin(), ruleIndex = 0;
        if (maxRuleIndex > 0) {
            // no rule can hold the allocation, merge the allocation into one
            ruleIndex = Random::getInstance().nextInt32(0, maxRuleIndex - 1);
            ruleIndex = findNearestUnvisited(ruleIndex);
        }
        if (ruleIndex >= isolateRuleSet.size() || maxRuleIndex == 0) {
            uint32_t sum = std::accumulate(allocation.begin(), allocation.end(), 0);
            allocation.clear();
            allocation.push_back(sum);
            ruleIndex = Random::getInstance().nextInt32(0, isolateRuleSet.size() - 1);
            ruleIndex = findNearestUnvisited(ruleIndex);
        }
        if (ruleIndex >= isolateRuleSet.size()) {
            // still no rule can hold the allocation, error
            throw NoRuleError();
        }
        visited[ruleIndex] = true;
        ruleSet.push_back(std::move(isolateRuleSet[ruleIndex]));
    }
    return ruleSet;
}

}