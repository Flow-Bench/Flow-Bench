#pragma once

// the third step of a recursive subproblem
// in the first step (QuadDag select), we have selected a QuadDag
// in the second step (VirtualRule select), we have selected some virtual rules
// however, we may select the same virtual rule multiple times
// e.g. we want to generate a DAG with D = 0 (no dependency), but there is only one virtual rule with d = 0
// in this case, we need to select the same virtual rule 4 times
// evidently, we cannot use the same virtual rule as the parent of next layer's search 4 times
//            otherwise we may generate the same rules multiple times in the result set
// therefore, we should "split" the selected virtual rule into 4 virtual rules
// e.g. 0 -> 000, 001, 010, and 011

#include "rule_virtual_selector.hpp"
#include "problem_state.hpp"

namespace flowbench {

static auto& virtualRuleIndexes = VirtualRuleSelector::getInstance().result;

class VirtualRuleSplitter : public Singleton<VirtualRuleSplitter> {
public:
    VirtualRuleSplitter() = default;

    // whether the splitted virtual rules allow wildcard in the next layer
    // 1. if the rule is "solid" and not split, then it is not allowed to have wildcard in the next layer
    // 2. if the rule is "solid" and split, then it is allowed to have wildcard in the next layer
    // 3. if the rule is "virtual", then it is allowed to have wildcard in the next layer
    std::vector<bool> allowWildcard;

    // split the virtual rules into a new rule set
    // n : state.n on the current layer
    // profile : the profile of the selected QuadDag
    // return : the mixed rule set of solid rules and splitted virtual rules
    //          if size of the result > 4, then there are virtual rules
    std::unique_ptr<CandidateRuleSet> split(const ProblemState& state, const QuadDagProfile& profile);

private:
    std::vector<uint32_t> counter;
    std::vector<bool> conflict;
};

std::unique_ptr<CandidateRuleSet> VirtualRuleSplitter::split(const ProblemState& state, const QuadDagProfile& profile) {
    uint32_t n = state.n, count = 0;
    auto result = std::make_unique<CandidateRuleSet>();
    if (n <= QD_VERTEX_CNT) {
        count = n;
    } else {
        count = QD_VERTEX_CNT + virtualRuleIndexes.size();
    }
    result->resize(count);
    const auto& solidRules = profile.getSolidRules();
    const auto& virtualRules = profile.getVirtualRules();
    for (uint32_t i = 0; i < std::min<uint32_t>(n, QD_VERTEX_CNT); i++) {
        result->at(i) = solidRules.getRule(i).clone();
    }
    if (n <= QD_VERTEX_CNT) {
        return result;
    }
    uint32_t conflictSolveFieldIndex = Random::getInstance().nextInt32(0, profile.getActualFieldCount() - 1);
    uint32_t conflictWidth = 0;
    uint32_t maxCounter = 0;
    counter.resize(virtualRules.size());
    conflict.resize(virtualRules.size());
    std::fill(counter.begin(), counter.end(), 0);
    std::fill(conflict.begin(), conflict.end(), false);
    allowWildcard.resize(virtualRuleIndexes.size());
    for (uint8_t i = 0; i < virtualRuleIndexes.size(); i++) {
        uint8_t index = virtualRuleIndexes[i];
        counter[index]++;
        if (counter[index] > 1) {
            conflict[index] = true;
        }
        if (counter[index] > maxCounter) {
            maxCounter = counter[index];
        }
    }
    while ((1 << conflictWidth) < maxCounter) {
        conflictWidth++;
    }
    for (uint8_t i = 0; i < virtualRuleIndexes.size(); i++) {
        uint8_t index = virtualRuleIndexes[i];
        auto rule = virtualRules.getRule(index).clone();
        if (conflictWidth > 0 && conflict[index]) {
            rule->getFieldAs<LpmField<Int32>>(conflictSolveFieldIndex).addSuffix(Int32(--counter[index]), conflictWidth);
        }
        result->at(i + QD_VERTEX_CNT) = std::move(rule);
        allowWildcard[i] = (!virtualRules.isSolid(index) || conflict[index]);
    }
    return result;
}

}