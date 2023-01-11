#pragma once

// a class to represent a profile of a QuadDag
// our pre-computed file is a set of QuadDag profiles

#include <istream>

#include "quad_dag_instantiater.hpp"
#include "quad_dag_virtualizer.hpp"

namespace flowbench {

class QuadDagProfile {
private:
    // the profile contains 2 parts
    // 1. rules: solid rules and virtual rules, with their d, e, and s
    // 2. global information: total D and E, and some other information

    std::unique_ptr<CandidateRuleSet> solidRules = nullptr;
    std::unique_ptr<CandidateRuleSet> virtualRules = nullptr;

    uint8_t totalDependencyLength = 0;
    uint8_t totalEdgeCount = 0;
    bool existWildcard = false;

    // there is 3 fields in a candidate rule, but not every field is used
    // if we want to generate rules with fewer fields (e.g. Destination Address only)
    // we must select the candidate rule sets whose actual field count is less than or equal to what we want
    uint8_t actualFieldCount;

    // the total bit width will be in [2, 5], and the bit width of each field will be in [0, 4]
    // if the bit width is not enough, we need to select the candidate rule sets which require fewer bits
    uint8_t totalBitWidth;
    std::array<uint8_t, QD_FIELD_CNT> fieldBitWidths;

public:
    QuadDagProfile() = default;
    QuadDagProfile(std::istream& is); // read from our pre-computed file

public:
    // we use this function to generate a profile from a QuadDag
    // analyzer: the QuadDag analyzer, representing the QuadDag we want to generate the profile from
    // instantiater: the QuadDag instantiater, used to generate the solid rules
    // virtualizer: the QuadDag virtualizer, used to generate the virtual rules
    bool generate(const QuadDagAnalyzer& analyzer, QuadDagInstantiater& instantiater, QuadDagVirtualizer& virtualizer);

private:
    // we use this function to generate the global information
    void generateGlobalInfo();

public:
    auto& getSolidRules() const {
        return *solidRules;
    }

    auto& getVirtualRules() const {
        return *virtualRules;
    }

    uint8_t getTotalDependencyLength() const {
        return totalDependencyLength;
    }

    uint8_t getTotalEdgeCount() const {
        return totalEdgeCount;
    }

    uint8_t getTotalParameter() const {
        if (Task::getInstance().getType() == TaskType::DependencyLength) {
            return getTotalDependencyLength();
        } else {
            return getTotalEdgeCount();
        }
    }

    bool getExistWildcard() const {
        return existWildcard;
    }

    uint8_t getActualFieldCount() const {
        return actualFieldCount;
    }

    uint8_t getTotalBitWidth() const {
        return totalBitWidth;
    }

    uint8_t getFieldBitWidth(uint8_t field) const {
        return fieldBitWidths.at(field);
    }

};

bool QuadDagProfile::generate(const QuadDagAnalyzer& analyzer, QuadDagInstantiater& instantiater, QuadDagVirtualizer& virtualizer) {
    // std::cout << "Generating profile..." << std::endl;
    solidRules = instantiater(analyzer);
    if (solidRules == nullptr) {
        return false;
    }
    // std::cout << "Instantiation done." << std::endl;
    solidRules->generateSolidRulesProfile(analyzer.getDag());
    // std::cout << "Solid rules profile done." << std::endl;
    virtualRules = virtualizer(*solidRules);
    virtualRules->generateVirtualRulesProfile(*solidRules);
    generateGlobalInfo();
    return true;
}

void QuadDagProfile::generateGlobalInfo() {
    existWildcard = solidRules->getRule(0).isWildcard();
    totalDependencyLength = totalEdgeCount = 0;
    for (uint8_t i = 0; i < QD_VERTEX_CNT; i++) {
        totalDependencyLength += solidRules->getDependencyLength(i);
        totalEdgeCount += solidRules->getEdgeCount(i);
    }
    actualFieldCount = totalBitWidth = 0;
    std::fill(fieldBitWidths.begin(), fieldBitWidths.end(), 0);
    for (uint8_t i = 0; i < QD_FIELD_CNT; i++) {
        for (uint8_t j = 0; j < QD_VERTEX_CNT; j++) {
            fieldBitWidths[i] = std::max(fieldBitWidths[i], solidRules->getRule(j).getFieldAs<LpmField<Int32>>(i).getPrefixLength());
        }
        if (fieldBitWidths[i] > 0) {
            actualFieldCount++;
            totalBitWidth += fieldBitWidths[i];
        }
    }
}

QuadDagProfile::QuadDagProfile(std::istream& is) {
    solidRules = std::make_unique<CandidateRuleSet>();
    virtualRules = std::make_unique<CandidateRuleSet>();
    std::string _, temp;
    is >> _;
    is >> _ >> totalDependencyLength;
    is >> _ >> totalEdgeCount;
    is >> _ >> existWildcard;
    is >> _ >> actualFieldCount;
    is >> _ >> totalBitWidth;
    is >> _;
    for (uint8_t i = 0; i < QD_FIELD_CNT; i++) {
        is >> fieldBitWidths[i];
    }
    while (is >> temp, temp != "END") {
        if (temp == "SR") {
            solidRules->readRule(is);
        } else if (temp == "VR") {
            virtualRules->readRule(is);
        }
    }
}

}