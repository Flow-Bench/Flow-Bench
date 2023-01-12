#pragma once

// the algorithm template to instantiate the QuadDag
// there is 2 algorithms inherited from this class:
// 1. normal QuadDag instantiation
// 2. dense QuadDag instantiation, which is used for dense mode
// this instantiate algorithm only instantiate the 4 solid rules for the QuadDag

// our goal is:
// 1. to find the 4 solid rules satisfying the QuadDag
// 2. minimize the used bit width of the 4 solid rules
// 3. minimize the used field number of the 4 solid rules (less priority than 2)
// so we use maxBitWidth to control the bit width of the 4 solid rules
// from our experiments, maxBitWidth can never exceed 4 for all QuadDags
// and the sum of the bit width of the 4 solid rules can never exceed 5

#include <numeric>

#include "rule_set_candidate.hpp"
#include "quad_dag_analyzer.hpp"

namespace flowbench {

const static uint8_t MAX_BIT_WIDTH = 4;
const static uint8_t MIN_SUM_BIT_WIDTH = 2;
const static uint8_t MAX_SUM_BIT_WIDTH = 5;

class QuadDagInstantiater {
protected:
    using Lpm32 = LpmField<Int32>;
    virtual uint8_t nextBitWidth(uint8_t bitWidth) const = 0;

private:
    // the possible fields for each field
    // possibleFields[i] = the possible fields for field i, i = 0, 1, 2 (Candidate rules have 3 LPM fields)
    std::array<std::vector<Lpm32>, QD_FIELD_CNT> possibleFields;

    // initialize the possible fields for each field
    // with *, 0, 00, 000, etc.
    void initializePossibleFields(uint8_t maxBitWidth);

    // extend the field with bits, the strategy may be different for normal and dense mode
    // result: possible fields at current, may be added with the extended fields as new possible fields
    // field: the field to extend, added to the candidate rule set in the search procedure
    void extend(std::vector<Lpm32>& result, const Lpm32& field, uint8_t maxBitWidth);
    void extend(const CandidateRule& rule, uint8_t maxBitWidth) {
        for (uint8_t i = 0; i < QD_FIELD_CNT; i++) {
            extend(possibleFields[i], rule.getFieldAs<Lpm32>(i), maxBitWidth);
        }
    }

    // we will instantiate the ruleSet with by a recursive procedure
    // ruleIndex: the index of the rule to instantiate
    // usedFieldCount: the number of fields used in the instantiated rules
    // fieldBitWidth: the bit width of the fields used in the instantiated rules
    bool instantiateRule(const QuadDagAnalyzer& analyzer, CandidateRuleSet& ruleSet, uint8_t ruleIndex, uint8_t usedFieldCount, std::array<uint8_t, QD_FIELD_CNT>& fieldBitWidth);

    // when we try to instantiate a rule, we will try to instantiate the fields of the rule
    bool nextFieldIndex(std::array<uint8_t, QD_FIELD_CNT>& fieldIndex, uint8_t usedFieldCount);

    // global variables to control the instantiation (to satisfy goal 2 and 3)
    uint8_t sumBitWidth, fieldCount;

public:
    // try to instantiate the QuadDag to a CandidateRuleSet
    // if the instantiation is successful, return the instantiated CandidateRuleSet
    // otherwise, return nullptr
    std::unique_ptr<CandidateRuleSet> operator()(const QuadDagAnalyzer& analyzer) {
        for (sumBitWidth = MIN_SUM_BIT_WIDTH; sumBitWidth <= MAX_SUM_BIT_WIDTH; sumBitWidth++) {
            for (fieldCount = 1; fieldCount <= QD_FIELD_CNT; fieldCount++) {
                auto result = std::make_unique<CandidateRuleSet>(QD_VERTEX_CNT);
                std::array<uint8_t, QD_FIELD_CNT> fieldBitWidth = {0};
                initializePossibleFields(std::min(MAX_BIT_WIDTH, sumBitWidth));
                if (instantiateRule(analyzer, *result, 0, 0, fieldBitWidth)) {
                    return result;
                }
            }
        }
        return nullptr;
    }

};

void QuadDagInstantiater::extend(std::vector<Lpm32>& result, const Lpm32& field, uint8_t maxBitWidth) {
    uint8_t curBitWidth = field.getPrefixLength();
    if (curBitWidth == 0) {
        return;
    }
    for (uint8_t i = 1; i <= curBitWidth; i++) {
        Int32 prefix = field.getPrefix() ^ (Int32(1) << (32 - i));
        for (uint8_t j = nextBitWidth(i); j <= maxBitWidth; j++) {
            if (std::find(result.begin(), result.end(), Lpm32(prefix, j)) == result.end()) {
                result.emplace_back(prefix, j);
            }
        }
    }
}

void QuadDagInstantiater::initializePossibleFields(uint8_t maxBitWidth) {
    for (uint8_t i = 0; i < QD_FIELD_CNT; i++) {
        possibleFields[i].clear();
        for (uint8_t j = 0; j <= maxBitWidth; j++) {
            possibleFields[i].emplace_back(Lpm32(0, j));
        }
    }
}

bool QuadDagInstantiater::nextFieldIndex(std::array<uint8_t, QD_FIELD_CNT>& fieldIndex, uint8_t usedFieldCount) {
    while (true) {
        bool allMax = true;
        for (uint8_t i = 0; i < fieldCount; i++) {
            if (fieldIndex[i] < possibleFields[i].size() - 1) {
                fieldIndex[i]++;
                allMax = false;
                break;
            }
            if (i < usedFieldCount) {
                fieldIndex[i] = 0;
            } else {
                fieldIndex[i] = 1;
            }
        }
        if (allMax) {
            return false;
        }
        uint8_t sum = 0;
        for (uint8_t i = 0; i < fieldCount; i++) {
            sum += possibleFields[i][fieldIndex[i]].getPrefixLength();
        }
        if (sum <= sumBitWidth) {
            return true;
        }
    }
    return false;
}

bool QuadDagInstantiater::instantiateRule(const QuadDagAnalyzer& analyzer, CandidateRuleSet& ruleSet, 
        uint8_t ruleIndex, uint8_t usedFieldCount, std::array<uint8_t, QD_FIELD_CNT>& fieldBitWidth) {
    if (ruleIndex == QD_VERTEX_CNT) { // we have instantiated all rules, return true
        return true;
    }
    auto& rule = ruleSet.getRule(ruleIndex);
    std::array<uint8_t, QD_FIELD_CNT> fieldIndex = {0};
    do {
        auto newFieldBitWidth = fieldBitWidth;
        uint8_t newUsedFieldCount = 0;
        for (uint8_t i = 0; i < fieldCount; i++) {
            if (fieldIndex[i] > 0) {
                newUsedFieldCount = std::max<uint8_t>(newUsedFieldCount, i + 1);
            }
            newFieldBitWidth[i] = std::max(newFieldBitWidth[i], possibleFields[i][fieldIndex[i]].getPrefixLength());
            rule.setField(i, possibleFields[i][fieldIndex[i]].clone());
        }
        if (ruleSet.isSorted(ruleIndex) && analyzer.checkSatisfy(ruleSet, ruleIndex)) {
            std::array<uint8_t, QD_FIELD_CNT> possibleFieldsSize = {0}; // update the possible fields
            for (uint8_t i = 0; i < QD_FIELD_CNT; i++) {
                possibleFieldsSize[i] = possibleFields[i].size();
                extend(possibleFields[i], rule.getFieldAs<Lpm32>(i), std::min(MAX_BIT_WIDTH, sumBitWidth));
            }
            if (instantiateRule(analyzer, ruleSet, ruleIndex + 1, newUsedFieldCount, newFieldBitWidth)) {
                return true;
            }
            for (uint8_t i = 0; i < QD_FIELD_CNT; i++) {
                possibleFields[i].resize(possibleFieldsSize[i]);
            }
        }
    } while (nextFieldIndex(fieldIndex, usedFieldCount));
    return false;
};

}