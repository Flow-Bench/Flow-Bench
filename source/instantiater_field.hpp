#pragma once

// the fifth step of a recursive subproblem
// in the first step (QuadDag select), we have selected a QuadDag
// in the second step (VirtualRule select), we have selected some virtual rules
// in the third step (VirtualRule split), we have generated a mixed rule set
// in the fourth step (Bit instantiater), we have applied xor operation to the rule set
// in this step, we will convert the candidate rule set to a user-defined rule set
// we will build a mapping from the candidate rule fields (3 32-bit LPM fields) to the user-defined rule fields
// 1. used fields in CRS -> available fields in URS (check the available widths)
// 2. randomly set EM fields in URS

#include "rule_set_candidate.hpp"
#include "random_selector.hpp"
#include "problem_state.hpp"
#include "quad_dag_profile.hpp"

namespace flowbench {

class FieldInstantiater : public Singleton<FieldInstantiater> {
public:
    FieldInstantiater();

    // convert CRS to URS
    // ruleSet: the candidate rule set (CRS) to be converted
    // state: the problem state
    // profile: the selected QuadDag profile
    // return: the converted user-defined rule set (URS)
    std::unique_ptr<UDRuleSet> operator()(const CandidateRuleSet& ruleSet, const ProblemState& state, const QuadDagProfile& profile);

private:
    // CRS index -> URS index
    std::array<uint8_t, QD_FIELD_CNT> mapping;

    // required field widths (CRS)
    std::array<uint8_t, QD_FIELD_CNT> requiredWidths;

    // try to locate fields with more required widths first
    std::array<uint8_t, QD_FIELD_CNT> requiredWidthsOrder;

    // field weights (URS)
    // will be set as state.fieldWeights at first
    // when we set EM fields, we will set the corresponding weights to 0
    std::vector<double> fieldWeights;

    // whether the field has set up a mapping (URS)
    std::vector<bool> fieldMapped;

    // instantiated EM fields (URS)
    std::vector<uint8_t> emFields;

};

FieldInstantiater::FieldInstantiater() {
    fieldWeights.resize(RuleTypeUD::getInstance().getFieldCount());
    fieldMapped.resize(RuleTypeUD::getInstance().getFieldCount());
}

std::unique_ptr<UDRuleSet> FieldInstantiater::operator()(const CandidateRuleSet& ruleSet, const ProblemState& state, const QuadDagProfile& profile) {
    std::fill(mapping.begin(), mapping.end(), 0);
    std::fill(requiredWidths.begin(), requiredWidths.end(), 0);
    std::copy(state.fieldWeights.begin(), state.fieldWeights.end(), fieldWeights.begin());
    std::fill(fieldMapped.begin(), fieldMapped.end(), false);
    emFields.clear();
    for (uint8_t i = 0; i < ruleSet.size(); i++) {
        for (uint8_t j = 0; j < profile.getActualFieldCount(); j++) {
            requiredWidths[j] = std::max(requiredWidths[j], ruleSet.getRule(i).getFieldAs<LpmField<Int32>>(j).getPrefixLength());
        }
    }
    for (uint8_t i = 0; i < QD_FIELD_CNT; i++) {
        requiredWidthsOrder[i] = i;
    }
    std::sort(requiredWidthsOrder.begin(), requiredWidthsOrder.begin() + profile.getActualFieldCount(), [this](uint8_t a, uint8_t b) {
        return requiredWidths[a] > requiredWidths[b];
    });
    for (uint8_t i = 0; i < profile.getActualFieldCount(); i++) {
        uint8_t field = requiredWidthsOrder[i];
        uint8_t width = requiredWidths[field];
        while (true) {
            uint8_t index = RandomSelector::getInstance().select(fieldWeights);
            fieldWeights[index] = 0;
            if (state.availableWidths[index] >= width) {
                mapping[field] = index;
                fieldMapped[index] = true;
                break;
            } else if (RuleTypeUD::getInstance().getMatchType(index) == MatchType::EM) {
                emFields.push_back(index);
            }
        }
    }
    for (uint8_t i = profile.getActualFieldCount(), j = 0; i < QD_FIELD_CNT; i++) {
        while (j < RuleTypeUD::getInstance().getFieldCount() && fieldMapped[j]) {
            j++;
        }
        if (j < RuleTypeUD::getInstance().getFieldCount()) {
            mapping[i] = j++;
        }
    }
    auto result = std::make_unique<UDRuleSet>();
    for (uint8_t i = 0; i < ruleSet.size(); i++) {
        auto rule = std::make_unique<UDRule>(ruleSet.getRule(i), mapping);
        result->push_back(std::move(rule));
    }
    for (uint8_t i = 0; i < emFields.size(); i++) {
        auto emField = RuleTypeUD::getInstance().createField(emFields[i])->clone();
        emField->randomize();
        for (uint8_t j = 0; j < result->size(); j++) {
            result->getRule(j).setField(emFields[i], emField->clone());
        }
    }
    return result;
}


}