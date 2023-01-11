#pragma once

#include "constants.hpp"
#include "singleton.hpp"
#include "rule_type.hpp"

namespace flowbench {

class RuleTypeCandidate : public RuleType, public Singleton<RuleTypeCandidate> {
public:
    uint8_t getFieldCount() const override {
        return QD_FIELD_CNT;
    }

    uint8_t getFieldWidth(uint8_t fieldIndex) const override {
        return 8;
    }

    MatchType getMatchType(uint8_t fieldIndex) const override {
        return MatchType::LPM;
    }

    RuleTypeCandidate() = default;

};

}