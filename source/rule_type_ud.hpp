#pragma once

// user-defined rule type

#include <vector>

#include "rule_type_ipv4.hpp"
#include "rule_type_ipv6.hpp"
#include "rule_type_openflow_1_0.hpp"
#include "protocol.hpp"

namespace flowbench {

class RuleTypeUD : public RuleType, public Singleton<RuleTypeUD> {
private:
    uint8_t fieldCount;
    std::vector<uint8_t> fieldWidths;
    std::vector<MatchType> matchTypes;

public:
    uint8_t getFieldCount() const override {
        return fieldCount;
    }

    uint8_t getFieldWidth(uint8_t fieldIndex) const override {
        return fieldWidths[fieldIndex];
    }

    MatchType getMatchType(uint8_t fieldIndex) const override {
        return matchTypes[fieldIndex];
    }

    RuleTypeUD() : fieldCount(0) {}

    void setFieldCount(uint8_t fieldCount) {
        constexpr static uint8_t DEFAULT_FIELD_COUNT = 5;
        constexpr static std::array<uint8_t, DEFAULT_FIELD_COUNT> DEFAULT_FIELD_WIDTHS = { 32, 32, 16, 16, 8 };
        constexpr static std::array<MatchType, DEFAULT_FIELD_COUNT> DEFAULT_MATCH_TYPES = { MatchType::LPM, MatchType::LPM, MatchType::RM, MatchType::RM, MatchType::EM };
        this->fieldCount = fieldCount;
        fieldWidths.resize(fieldCount);
        matchTypes.resize(fieldCount);
        for (uint8_t i = 0; i < fieldCount; i++) {
            fieldWidths[i] = i < DEFAULT_FIELD_COUNT ? DEFAULT_FIELD_WIDTHS[i] : 8;
            matchTypes[i] = i < DEFAULT_FIELD_COUNT ? DEFAULT_MATCH_TYPES[i] : MatchType::EM;
        }
    }

    void setFieldWidth(uint8_t fieldIndex, uint8_t fieldWidth) {
        fieldWidths[fieldIndex] = fieldWidth;
    }

    void setMatchType(uint8_t fieldIndex, MatchType matchType) {
        matchTypes[fieldIndex] = matchType;
    }

    void setProtocol(Protocol protocol) {
        RuleType* ruleType = nullptr;
        switch (protocol) {
            case Protocol::IPv4:
                ruleType = &RuleTypeIPv4::getInstance();
                break;
            case Protocol::IPv6:
                ruleType = &RuleTypeIPv6::getInstance();
                break;
            case Protocol::OpenFlow1_0:
                ruleType = &RuleTypeOpenFlow1_0::getInstance();
                break;
            default:
                break;
        }
        if (ruleType != nullptr) {
            fieldCount = ruleType->getFieldCount();
            fieldWidths.resize(fieldCount);
            matchTypes.resize(fieldCount);
            for (uint8_t i = 0; i < fieldCount; i++) {
                fieldWidths[i] = ruleType->getFieldWidth(i);
                matchTypes[i] = ruleType->getMatchType(i);
            }
        }
    }

};

}