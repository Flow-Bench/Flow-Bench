#pragma once

// a flow with User-defined fields

#include "rule.hpp"

namespace flowbench {

class Flow {
private:
    std::vector<std::unique_ptr<Integer>> fields;

public:
    Flow() = default;
    Flow(uint8_t fieldCount): fields(fieldCount) {}
    Flow(const UDRule& rule);

    Flow clone() const {
        Flow flow(fields.size());
        for (uint8_t i = 0; i < fields.size(); i++) {
            flow.fields.push_back(fields[i]->clone());
        }
        return flow;
    }

    const Integer& getField(uint8_t index) const {
        return *fields[index];
    }
};

// generate an exact match flow from a rule
Flow::Flow(const UDRule& rule) {
    for (uint8_t i = 0; i < rule.getFieldCount(); i++) {
        fields.push_back(rule.getField(i).hit());
    }
}

}