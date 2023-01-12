#pragma once

// isolate rule set
// for trace generator, we need to split the rules into isolate rules
// note: if you generated a very large rule set, and the user-defined protocol has a lot of fields,
//       our algorithm may be very slow and consume a lot of memory
//       because its time complexity is O(n^2 * m), where n is the number of rules, and m is the number of fields
//       in this case, if you do not need the rule-level spatial locality, you can use the fast mode to omit this step

#include "rule_set.hpp"

namespace flowbench {

class IsolateRuleSet : public UDRuleSet {
private:
    std::vector<uint8_t> differentFields;
    std::vector<std::vector<std::unique_ptr<MatchField>>> differentFieldValues;

    bool getDifferentFields(const UDRule& r1, const UDRule& r2);
    bool getDifferentFieldValues(const UDRule& r1, const UDRule& r2);
    void makeCartesianProduct(const UDRule& r, IsolateRuleSet& out) const;

public:
    IsolateRuleSet() = default;
    IsolateRuleSet(const UDRule& rule) {
        push_back(rule.clone());
    }
    IsolateRuleSet(UDRuleSet&& ruleSet) {
        *this = std::move(ruleSet);
    }

    // split every rule r in the rule set by the given rule
    // if r overlaps with the given rule:
    // 1. find the different fields between r and the given rule
    // 2. calculate the difference of the different fields
    // 3. make a Cartesian product of the different fields
    // if cannot split, return false
    bool splitBy(const UDRule& rule);
};

bool IsolateRuleSet::splitBy(const UDRule& rule) {
    IsolateRuleSet temp;
    for (uint32_t i = 0; i < size(); i++) {
        auto& r = getRule(i);
        if (getDifferentFields(r, rule)) {
            if (getDifferentFieldValues(r, rule)) {
                makeCartesianProduct(r, temp);
            }
        } else {
            temp.push_back(r.clone());
        }
    }
    *this = std::move(temp);
    if (size() == 0) {
        return false;
    }
    return true;
}

bool IsolateRuleSet::getDifferentFields(const UDRule& r1, const UDRule& r2) {
    bool overlap = true;
    differentFields.clear();
    for (uint8_t j = 0; j < r1.getFieldCount(); j++) {
        if (r1.getField(j).overlap(r2.getField(j))) {
            if (r1.getField(j) != r2.getField(j)) {
                differentFields.push_back(j);
            }
        } else {
            overlap = false;
            break;
        }
    }
    return overlap;
}

bool IsolateRuleSet::getDifferentFieldValues(const UDRule& r1, const UDRule& r2) {
    differentFieldValues.clear();
    for (uint8_t j = 0; j < differentFields.size(); j++) {
        std::vector<std::unique_ptr<MatchField>> values;
        const auto& f1 = r1.getField(differentFields[j]);
        const auto& f2 = r2.getField(differentFields[j]);
        if (!f1.difference(f2, values)) {
            return false;
        }
        differentFieldValues.push_back(std::move(values));
    }
    return true;
}

void IsolateRuleSet::makeCartesianProduct(const UDRule& r, IsolateRuleSet& out) const {
    auto rule = r.clone();
    std::vector<uint8_t> indexes(differentFields.size(), 0);
    while (true) {
        for (uint8_t i = 0; i < differentFields.size(); i++) {
            rule->setField(differentFields[i], differentFieldValues[i][indexes[i]]->clone());
        }
        out.push_back(rule->clone());
        for (int8_t i = 0; i < differentFields.size(); i++) {
            if (indexes[i] < differentFieldValues[i].size() - 1) {
                indexes[i]++;
                break;
            } else {
                indexes[i] = 0;
                if (i == differentFields.size() - 1) {
                    return;
                }
            }
        }
    }
}

}