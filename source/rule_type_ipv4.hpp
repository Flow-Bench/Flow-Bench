#pragma once

#include "rule_type.hpp"

namespace flowbench {

class RuleTypeIPv4 : public RuleType, public Singleton<RuleTypeIPv4> {
public:
    uint8_t getFieldCount() const override {
        return 5;
    }

    uint8_t getFieldWidth(uint8_t fieldIndex) const override {
        switch (fieldIndex) {
            case 0:
                return 32;
            case 1:
                return 32;
            case 2:
                return 16;
            case 3:
                return 16;
            case 4:
                return 8;
            default:
                return 0;
        }
    }

    MatchType getMatchType(uint8_t fieldIndex) const override {
        switch (fieldIndex) {
            case 0:
            case 1:
                return MatchType::LPM;
            case 2:
            case 3:
                return MatchType::RM;
            case 4:
                return MatchType::EM;
            default:
                return MatchType::EM;
        }
    }

    RuleTypeIPv4() = default;    
};

}