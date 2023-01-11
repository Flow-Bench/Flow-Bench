#pragma once

// base class for rule types
// there is 2 types of rules in FlowBench
// 1. RuleTypeCandidate : 3 8-bit LPM fields, used for representing pre-computed QuadDags
// 2. RuleTypeUd        : User-Defined Type

#include <cstdint>
#include <vector>
#include <memory>

#include "match_field_em.hpp"
#include "match_field_lpm.hpp"
#include "match_field_rm.hpp"

#include "int32.hpp"
#include "int64.hpp"
#include "int128.hpp"

namespace flowbench {

class RuleType {
public:
    virtual uint8_t getFieldCount() const = 0;
    virtual uint8_t getFieldWidth(uint8_t fieldIndex) const = 0;
    virtual MatchType getMatchType(uint8_t fieldIndex) const = 0;

    uint8_t getAvailableBitCount() const {
        uint8_t count = 0;
        for (uint8_t i = 0; i < getFieldCount(); i++) {
            if (getMatchType(i) != MatchType::EM) {
                count += getFieldWidth(i);
            }
        }
        return count;
    }

private:
    std::vector<std::unique_ptr<MatchField>> matchFields;

public:
    std::unique_ptr<MatchField> createField(uint8_t fieldIndex) {
        while (matchFields.size() <= fieldIndex) {
            matchFields.push_back(nullptr);
        }
        if (matchFields[fieldIndex] == nullptr) {
            switch (getMatchType(fieldIndex)) {
            case MatchType::EM:
                if (getFieldWidth(fieldIndex) <= 32) {
                    matchFields[fieldIndex] = std::make_unique<EmField<Int32>>();
                } else if (getFieldWidth(fieldIndex) <= 64) {
                    matchFields[fieldIndex] = std::make_unique<EmField<Int64>>();
                } else {
                    matchFields[fieldIndex] = std::make_unique<EmField<Int128>>();
                }
                break;
            case MatchType::LPM:
                if (getFieldWidth(fieldIndex) <= 32) {
                    matchFields[fieldIndex] = std::make_unique<LpmField<Int32>>();
                } else if (getFieldWidth(fieldIndex) <= 64) {
                    matchFields[fieldIndex] = std::make_unique<LpmField<Int64>>();
                } else {
                    matchFields[fieldIndex] = std::make_unique<LpmField<Int128>>();
                }
                break;
            case MatchType::RM:
                if (getFieldWidth(fieldIndex) <= 32) {
                    matchFields[fieldIndex] = std::make_unique<RmField<Int32>>();
                } else if (getFieldWidth(fieldIndex) <= 64) {
                    matchFields[fieldIndex] = std::make_unique<RmField<Int64>>();
                } else {
                    matchFields[fieldIndex] = std::make_unique<RmField<Int128>>();
                }
                break;
            }
        }
        return matchFields[fieldIndex]->clone();
    }
};

}