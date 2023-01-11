#pragma once

// split a rule to 2 rules
// randomly select a LPM/RM field and split the field into 2 parts

#include "configuration.hpp"
#include "rule.hpp"
#include "random_selector.hpp"

namespace flowbench {

class RuleSplitter : public Singleton<RuleSplitter> {
public:
    RuleSplitter() = default;

    // split a rule into 2 rules
    std::pair<std::unique_ptr<UDRule>, std::unique_ptr<UDRule>> split(const UDRule &rule) const;
};

std::pair<std::unique_ptr<UDRule>, std::unique_ptr<UDRule>> RuleSplitter::split(const UDRule &rule) const {
    try {
        std::vector<double> fieldWeights;
        for (uint8_t i = 0; i < rule.getFieldCount(); i++) {
            if (rule.getAvailableWidth(i) > 0) {
                fieldWeights.push_back(Configuration::getInstance().getFieldWeight(i));
            } else {
                fieldWeights.push_back(0);
            }
        }
        uint8_t fieldIndex = RandomSelector::getInstance().select(fieldWeights);
        auto left = rule.clone();
        auto right = rule.clone();
        left->getField(fieldIndex).addSuffix(0, 1);
        right->getField(fieldIndex).addSuffix(1, 1);
        return std::make_pair(std::move(left), std::move(right));
    } catch (const std::exception &e) {
        return std::make_pair(nullptr, nullptr);
    }
}

}