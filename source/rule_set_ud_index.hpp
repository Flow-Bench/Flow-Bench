#pragma once

// user-defined rule set with index
// for trace generator, every flow has a rule index

#include "rule_set.hpp"

namespace flowbench {

class UDRuleSetWithIndex : public UDRuleSet {
private:
    std::vector<uint32_t> ruleIndex;

public:
    void push_back(std::unique_ptr<UDRule>&& rule, uint32_t index) {
        UDRuleSet::push_back(std::move(rule));
        ruleIndex.push_back(index);
    }

    uint32_t getRuleIndex(uint32_t index) const {
        return ruleIndex[index];
    }

};


}