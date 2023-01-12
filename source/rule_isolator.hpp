#pragma once

// the first step to generate traces
// we divide the rule pool into isolate rules which do not overlap with each other
// note: if you generated a very large rule set, and the user-defined protocol has a lot of fields,
//       our algorithm may be very slow and consume a lot of memory
// if rule-level spatial locality is not specified
//       or fast mode is enabled, we omit this step and use the original rule pool

#include "rule_pool.hpp"
#include "rule_set_isolate.hpp"

namespace flowbench {

class RuleIsolator : public Singleton<RuleIsolator> {
public:
    RuleIsolator() = default;

    std::unique_ptr<UDRuleSet> operator()();
};

std::unique_ptr<UDRuleSet> RuleIsolator::operator()() {
    auto ruleSet = std::make_unique<UDRuleSet>();
    bool enableFastMode = TraceConfiguration::getInstance().enableFastMode();
    if (!enableFastMode) {
        std::vector<IsolateRuleSet> isolateRuleSets;
        for (const auto& rule : RulePool::getInstance()) {
            for (auto& isolateRuleSet : isolateRuleSets) {
                if (!isolateRuleSet.splitBy(*rule)) {
                    enableFastMode = true;
                    break;
                }
            }
            isolateRuleSets.emplace_back(*rule);
            if (enableFastMode) {
                break;
            }
        }
        if (!enableFastMode) {
            for (auto& isolateRuleSet : isolateRuleSets) {
                ruleSet->push_back(std::move(isolateRuleSet[0]));
            }
        } else {
            ruleSet->clear();
        }
    }
    if (enableFastMode) {
        for (const auto& rule : RulePool::getInstance()) {
            ruleSet->push_back(rule->clone());
        }
    }
    ruleSet->sortByAvailableWidth();
    return ruleSet;
}

}