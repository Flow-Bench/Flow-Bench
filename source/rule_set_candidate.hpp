#pragma once

// candidate rule set is a extended rule set containing dependency length, edge number and solid information
// part of the QuadDag profile in the pre-computed file

#include <algorithm>
#include <iostream>

#include "task.hpp"
#include "rule_set.hpp"
#include "quad_dag.hpp"
#include "rule_output.hpp"

namespace flowbench {

class CandidateRuleSet : public RuleSet<RuleTypeCandidate> {
private:
    std::vector<uint8_t> dependencyLength;
    std::vector<uint8_t> edgeCount;
    std::vector<bool> solid;

public:
    CandidateRuleSet() = default;
    CandidateRuleSet(uint32_t ruleCount) : RuleSet<RuleTypeCandidate>(ruleCount) {}

public:
    // we use the following functions to generate the dependency length, edge count and solid information
    // note that it is different from solid rules and virtual rules
    // for all rules:
    // 1. dependency length: the length of the longest path to the rule
    // 2. edge count: the number of edges pointing to the rule
    // for solid rules:
    // 3. solid: true
    // for virtual rules:
    // 4. solid: whether the rule equals to a solid rule

    void generateSolidRulesProfile(const QuadDag& dag);
    void generateVirtualRulesProfile(const CandidateRuleSet& solidRules);

public:
    // read a candidate rule set from our pre-computed file
    void readRule(std::istream& is);

    // output the candidate rule set to a file
    // prefix: the prefix of every candidate rule
    // 1. for solid rules, the prefix is "S"
    // 2. for virtual rules, the prefix is "V"
    void printRules(std::ostream& os, std::string prefix) const;

public:
    uint8_t getDependencyLength(uint8_t index) const {
        return dependencyLength.at(index);
    }

    uint8_t getEdgeCount(uint8_t index) const {
        return edgeCount.at(index);
    }

    uint8_t getParameter(uint8_t index) const {
        if (Task::getInstance().getType() == TaskType::DependencyLength) {
            return getDependencyLength(index);
        } else {
            return getEdgeCount(index);
        }
    }

    bool isSolid(uint8_t index) const {
        return solid.at(index);
    }

    uint8_t getMinParameter() const {
        if (Task::getInstance().getType() == TaskType::DependencyLength) {
            return *std::min_element(dependencyLength.begin(), dependencyLength.end());
        } else {
            return *std::min_element(edgeCount.begin(), edgeCount.end());
        }
    }

    uint8_t getMaxParameter() const {
        if (Task::getInstance().getType() == TaskType::DependencyLength) {
            return *std::max_element(dependencyLength.begin(), dependencyLength.end());
        } else {
            return *std::max_element(edgeCount.begin(), edgeCount.end());
        }
    }

    void setDependencyLength(uint8_t index, uint8_t length) {
        dependencyLength.at(index) = length;
    }

    void setedgeCount(uint8_t index, uint8_t number) {
        edgeCount.at(index) = number;
    }
};

void CandidateRuleSet::generateSolidRulesProfile(const QuadDag& dag) {
    dependencyLength.resize(size());
    edgeCount.resize(size());
    solid.resize(size());
    std::fill(solid.begin(), solid.end(), true); // all rules are solid rules
    for (uint8_t i = 0; i < size(); i++) {
        dependencyLength.at(i) = 0;
        edgeCount.at(i) = 0;
        for (uint8_t j = 0; j < i; j++) {
            if (dag.getEdge(j, i) != EdgeType::None) {
                dependencyLength.at(i) = std::max<uint8_t>(dependencyLength.at(i), dependencyLength.at(j) + 1);
                edgeCount.at(i) ++;
            }
        }
    }
}

void CandidateRuleSet::generateVirtualRulesProfile(const CandidateRuleSet& solidRules) {
    dependencyLength.resize(size());
    edgeCount.resize(size());
    solid.resize(size());
    std::fill(solid.begin(), solid.end(), false); // all rules are not equal to any solid rules at first
    for (uint8_t i = 0; i < size(); i++) {
        dependencyLength.at(i) = 0;
        edgeCount.at(i) = 0;
        for (uint8_t j = 0; j < solidRules.size(); j++) {
            if (solidRules.getRule(j).getEdgeTypeTo(getRule(i)) != EdgeType::None) {
                dependencyLength.at(i) = std::max<uint8_t>(dependencyLength.at(i), solidRules.getDependencyLength(j) + 1);
                edgeCount.at(i) ++;
            }
            if (solidRules.getRule(j) == getRule(i)) {
                solid.at(i) = true;
            }
        }
    }
}

void CandidateRuleSet::readRule(std::istream& is) {
    auto rule = std::make_unique<CandidateRule>();
    std::string _, temp;
    uint32_t x;
    for (uint8_t i = 0; i < QD_FIELD_CNT; i++) {
        is >> temp;
        rule->setField(i, std::make_unique<LpmField<Int32>>(temp));
    }
    push_back(std::move(rule));
    is >> _ >> x;
    dependencyLength.push_back(x);
    is >> _ >> x;
    edgeCount.push_back(x);
    is >> _ >> x;
    solid.push_back(x == 1);
}

void CandidateRuleSet::printRules(std::ostream& os, std::string prefix) const {
    for (uint8_t i = 0; i < size(); i++) {
        os << prefix << getRule(i) << " ";
        os << " d= " << (int) dependencyLength.at(i) << " ";
        os << " e= " << (int) edgeCount.at(i) << " ";
        os << " s= " << (int) solid.at(i) << "\n";
    }
}

}