#pragma once

// a class for rule set
// when use RuleSet(ruleCount) to create a rule set
// we do not create rule objects (must use reset to create rule objects)

#include <algorithm>

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
        std::vector<std::pair<uint32_t, uint32_t>> ruleWidths;
        for (uint32_t i = 0; i < this->size(); i++) {
            ruleWidths.push_back(std::make_pair(i, getRule(i).getAvailableWidth()));
        }
        std::sort(ruleWidths.begin(), ruleWidths.end(), [](const std::pair<uint32_t, uint32_t>& a, const std::pair<uint32_t, uint32_t>& b) {
            return a.second < b.second;
        });
        std::vector<std::unique_ptr<Rule<T>>> rules;
        for (uint32_t i = 0; i < this->size(); i++) {
            rules.push_back(std::move(this->at(ruleWidths[i].first)));
        }
        this->swap(rules);
    }

};

using UDRuleSet = RuleSet<RuleTypeUD>;

}