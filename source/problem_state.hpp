#pragma once

// the FlowBench algorithm is based on a recursive tree
// this class is used to represent a node in the tree
// that is, a problem state containing the recursive information

#include "rule.hpp"
#include "configuration.hpp"

namespace flowbench {

class ProblemState {
public:
    uint32_t n; // the number of rules
    uint32_t p; // the parameter (D or E) of the problem
    uint8_t k; // the available number of fields
    bool allowWildcard; // whether wildcard is allowed (the parent is not solid)
    std::unique_ptr<UDRule> parent; // the parent rule
    std::vector<uint8_t> availableWidths; // the available widths of the fields
    std::vector<double> fieldWeights; // the weights of the fields

    // the weights are set as -fwt at the beginning
    // and during the search, if all bits of a field have been used, the weight is set to 0

public:
    ProblemState(uint32_t n, uint32_t p, bool allowWildcard, std::unique_ptr<UDRule> parent);
};

ProblemState::ProblemState(uint32_t n, uint32_t p, bool allowWildcard, std::unique_ptr<UDRule> parent) :
    n(n), p(p), allowWildcard(allowWildcard), parent(std::move(parent)) {
    auto f = RuleTypeUD::getInstance().getFieldCount();
    k = 0;
    availableWidths.resize(f);
    fieldWeights.resize(f);
    for (uint8_t i = 0; i < f; i++) {
        fieldWeights[i] = Configuration::getInstance().getFieldWeight(i);
        if (RuleTypeUD::getInstance().getMatchType(i) == MatchType::EM && !this->parent->getField(i).isWildcard()) {
            fieldWeights[i] = 0;
        } else {
            availableWidths[i] = this->parent->getAvailableWidth(i);
            if (availableWidths[i] > 1 && fieldWeights[i] > 0) {
                k++;
            }
        }
    }
}

}
