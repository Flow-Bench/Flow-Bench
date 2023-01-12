#pragma once

// a flow with User-defined fields

#include "rule.hpp"

namespace flowbench {

class Flow {
private:
    std::vector<std::unique_ptr<Integer>> fields;
    uint32_t ruleIndex = 0;

public:
    Flow() = default;
    Flow(uint8_t fieldCount): fields(fieldCount) {}
    Flow(const UDRule& rule, uint32_t ruleIndex);

    std::unique_ptr<Flow> clone() const {
        auto flow = std::make_unique<Flow>();
        flow->fields.resize(fields.size());
        flow->ruleIndex = ruleIndex;
        for (uint8_t i = 0; i < fields.size(); i++) {
            flow->fields[i] = fields[i]->clone();
        }
        return flow;
    }

    const Integer& getField(uint8_t index) const {
        return *fields[index];
    }

    uint32_t getRuleIndex() const {
        return ruleIndex;
    }
};

// generate an exact match flow from a rule
Flow::Flow(const UDRule& rule, uint32_t ruleIndex) {
    for (uint8_t i = 0; i < rule.getFieldCount(); i++) {
        fields.push_back(rule.getField(i).hit());
    }
    this->ruleIndex = ruleIndex;
}

}