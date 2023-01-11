#pragma once

// the sixth step of a recursive subproblem
// in the first step (QuadDag select), we have selected a QuadDag
// in the second step (VirtualRule select), we have selected some virtual rules
// in the third step (VirtualRule split), we have generated a mixed rule set
// in the fourth step (Bit instantiater), we have applied xor operation to the rule set
// in the fifth step (Field instantiater), we have converted the candidate rule set to a user-defined rule set
// in this step, we will concatenate the parent virtual rule and the rules we have generated
// e.g. parent = 1,* + child = 1,0 -> result = 11,0

#include "rule_set.hpp"

namespace flowbench {

class RuleInstantiater : public Singleton<RuleInstantiater> {
public:
    RuleInstantiater() = default;
    void operator()(UDRuleSet& ruleSet, const UDRule& parent) const;
};

void RuleInstantiater::operator()(UDRuleSet& ruleSet, const UDRule& parent) const {
    for (uint8_t i = 0; i < RuleTypeUD::getInstance().getFieldCount(); i++) {
        for (uint8_t j = 0; j < ruleSet.size(); j++) {
            ruleSet.getRule(j).getField(i).setParent(parent.getField(i));
        }
    }
}

}