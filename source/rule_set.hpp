#pragma once

// a class for rule set
// when use RuleSet(ruleCount) to create a rule set
// we do not create rule objects (must use reset to create rule objects)

#include "rule.hpp"

namespace flowbench {

template <class T> // where T : RuleType
class RuleSet : public std::vector<std::unique_ptr<Rule<T>>> {
public:
    Rule<T>& getRule(uint32_t index) const {
        return *this->at(index);
    }

public:
    RuleSet() = default;
    RuleSet(uint32_t ruleCount): std::vector<std::unique_ptr<Rule<T>>>(ruleCount) {
        for (uint32_t i = 0; i < ruleCount; i++) {
            this->at(i) = std::make_unique<Rule<T>>();
        }
    }

public:
    // we should guarantee that the rule set is sorted
    // that is, if rule i covers rule j, then i < j
    // this function is used to check if the rule set is sorted at rule[index]
    bool isSorted(uint32_t index) const {
        for (uint32_t i = 0; i < index; i++) {
            if (getRule(index).cover(getRule(i))) {
                return false;
            }
        }
        return true;
    }

public:
    void sortByAvailableWidth() {
        std::sort(this->begin(), this->end(), [](const std::unique_ptr<Rule<T>>& a, const std::unique_ptr<Rule<T>>& b) {
            return a->getAvailableWidth() > b->getAvailableWidth();
        });
    }

};

using UDRuleSet = RuleSet<RuleTypeUD>;

}