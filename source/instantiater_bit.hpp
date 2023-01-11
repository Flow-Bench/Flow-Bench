#pragma once

// the fourth step of a recursive subproblem
// in the first step (QuadDag select), we have selected a QuadDag
// in the second step (VirtualRule select), we have selected some virtual rules
// in the third step (VirtualRule split), we have generated a mixed rule set
// from this step, we will try to generate the final rule set
// in bit instantiater, we will randomly generate a mask for each field
// and apply xor operation to the mask and every rule in the rule set
// so that we can generate a new rule set with different bits

#include "rule_set_candidate.hpp"
#include "random.hpp"

namespace flowbench {

class BitInstantiater : public Singleton<BitInstantiater> {
private:
    std::array<uint32_t, QD_FIELD_CNT> masks;

public:
    BitInstantiater() = default;
    void operator()(CandidateRuleSet& ruleSet);
};

void BitInstantiater::operator()(CandidateRuleSet& ruleSet) {
    for (uint8_t i = 0; i < QD_FIELD_CNT; i++) {
        masks[i] = Random::getInstance().nextUInt32();
    }
    for (uint8_t i = 0; i < ruleSet.size(); i++) {
        for (uint8_t j = 0; j < QD_FIELD_CNT; j++) {
            ruleSet.getRule(i).getFieldAs<LpmField<Int32>>(j) ^= masks[j];
        }
    }
}

}