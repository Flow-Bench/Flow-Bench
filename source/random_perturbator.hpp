#pragma once

// the seventh step of a recursive subproblem (optional)
// in the first step (QuadDag select), we have selected a QuadDag
// in the second step (VirtualRule select), we have selected some virtual rules
// in the third step (VirtualRule split), we have generated a mixed rule set
// in the fourth step (Bit instantiater), we have applied xor operation to the rule set
// in the fifth step (Field instantiater), we have converted the candidate rule set to a user-defined rule set
// in the sixth step (Rule instantiater), we have concatenated the parent virtual rule and the rules we have generated
// after the steps above, we have generated a mixed rule set of User-Defined rules
//                        which can be used to generate a final rule set
// if the user enable Arbitrary Range, FlowBench will add this step to the procedure
// in this step, we will randomly apply a perturbation to the RM fields
//               so that we can get more diverse rule sets
// this step will harm the performance of the algorithm (about 3x)
// and in some cases (e.g. TCAM) arbitrary range will not be supported by the hardware
// so we recommend the user to disable this step if not necessary

#include "configuration.hpp"
#include "rule_set.hpp"
#include "random.hpp"

namespace flowbench {

class RandomPerturbator : public Singleton<RandomPerturbator> {
public:
    RandomPerturbator() = default;
    void operator()(UDRuleSet& ruleSet, const UDRule& parent) const;
};

void RandomPerturbator::operator()(UDRuleSet& ruleSet, const UDRule& parent) const {
    if (Configuration::getInstance().isEnableArbitraryRange()) {
        for (uint8_t i = 0; i < RuleTypeUD::getInstance().getFieldCount(); i++) {
            if (RuleTypeUD::getInstance().getMatchType(i) == MatchType::RM) {
                const auto& parentRm = parent.getFieldAs<RmField<Int32>>(i);
                int64_t parentMin = parentRm.getMin().getValue();
                int64_t parentMax = parentRm.getMax().getValue();
                for (uint8_t j = 0; j < ruleSet.size(); j++) {
                    auto& rule = ruleSet.getRule(j);
                    auto newRule = rule.clone();
                    auto& rm = rule.getFieldAs<RmField<Int32>>(i);
                    int64_t min = rm.getMin().getValue();
                    int64_t max = rm.getMax().getValue();
                    int64_t range = max - min + 1;
                    for (int64_t r = range / 4; r > 0; r /= 2) {
                        int64_t offsetMin = Random::getInstance().nextInt32(-r, r);
                        int64_t offsetMax = Random::getInstance().nextInt32(-r, r);
                        uint32_t newMin = std::max(min + offsetMin, parentMin);
                        uint32_t newMax = std::min(max + offsetMax, parentMax);
                        newRule->setField(i, std::make_unique<RmField<Int32>>(newMin, newMax));
                        bool isValid = true;
                        for (uint8_t k = 0; k < ruleSet.size(); k++) {
                            if (k == j) {
                                continue;
                            }
                            const auto& otherRule = ruleSet.getRule(k);
                            if (otherRule.overlap(rule) != otherRule.overlap(*newRule)) {
                                isValid = false;
                                break;
                            }
                        }
                        if (isValid) {
                            ruleSet.at(j) = std::move(newRule);
                            break;
                        }
                    }
                }
            }
        }
    }
}

}