#pragma once

// read a rule set from the input file

#include "configuration_trace.hpp"
#include "rule_input.hpp"
#include "rule_set.hpp"

namespace flowbench {

class RulePool : public UDRuleSet, public Singleton<RulePool> {
public:
    RulePool() = default;
    RulePool(std::istream& is) {
        readRules(is);
        TraceConfiguration::getInstance().setRuleCount(size());
    }

private:
    void readRules(std::istream& is);
};

void RulePool::readRules(std::istream& is) {
    std::unique_ptr<UDRule> rule = std::make_unique<UDRule>();
    while (is >> *rule) {
        push_back(std::move(rule));
        rule = std::make_unique<UDRule>();
    }
}

}