#pragma once

// analyze a QuadDag
// part of the QuadDag search algorithm
// note: the analysis of QuadDag attributes is not included
//       it is included in QuadDag profile class
// the analysis here only serves the instantiation of a valid candidate rule set

#include "quad_dag.hpp"
#include "rule_set_candidate.hpp"

namespace flowbench {

class QuadDagAnalyzer {
private:
    QuadDag dag;

public:
    QuadDagAnalyzer() = default;

    const QuadDag& getDag() const {
        return dag;
    }

public:
    bool check() const;
    bool checkSatisfy(const CandidateRuleSet &ruleSet, uint8_t dst) const;
    bool next();
};

// check if the QuadDag is valid
// avoid QuadDags which cannot be instantiated to a valid candidate rule set
bool QuadDagAnalyzer::check() const {
    static constexpr std::array<std::tuple<uint8_t, uint8_t, uint8_t>, QD_VTRIAN_CNT> VTRIANS = {
        std::make_tuple(0, 1, 2),
        std::make_tuple(0, 1, 3),
        std::make_tuple(0, 2, 3),
        std::make_tuple(1, 2, 3)
    };
    for (auto vtrian : VTRIANS) {
        uint8_t a = std::get<0>(vtrian);
        uint8_t b = std::get<1>(vtrian);
        uint8_t c = std::get<2>(vtrian);
        auto ab = dag.getEdge(a, b);
        auto ac = dag.getEdge(a, c);
        auto bc = dag.getEdge(b, c);
        if ((ab == EdgeType::Cover && bc == EdgeType::Cover && ac != EdgeType::Cover) ||
            (ab == EdgeType::None  && bc == EdgeType::Cover && ac != EdgeType::None ) ||
            (ab == EdgeType::Cover && bc != EdgeType::None  && ac == EdgeType::None ) ||
            (ab == EdgeType::None  && bc != EdgeType::None  && ac == EdgeType::Cover)) {
            return false;
        }
    }
    return true;
}

// check if the QuadDag satisfies a candidate rule set
// check the edges pointing to dst only
bool QuadDagAnalyzer::checkSatisfy(const CandidateRuleSet &ruleSet, uint8_t dst) const {
    for (uint8_t i = 0; i < dst; i++) {
        if (dag.getEdge(i, dst) != ruleSet.getRule(i).getEdgeTypeTo(ruleSet.getRule(dst))) {
            return false;
        }
    }
    return true;
}

// step the QuadDag to the next state
// if we have reached the end, return false
// otherwise, return true
bool QuadDagAnalyzer::next() {
    for (uint8_t i = 0; i < QD_VPAIR_CNT; i++) {
        auto edge = dag.getEdge(i);
        if (edge == EdgeType::None) {
            dag.setEdge(i, EdgeType::Overlap);
            return true;
        } else if (edge == EdgeType::Overlap) {
            dag.setEdge(i, EdgeType::Cover);
            return true;
        }
        dag.setEdge(i, EdgeType::None);
    }
    return false;
}

}
