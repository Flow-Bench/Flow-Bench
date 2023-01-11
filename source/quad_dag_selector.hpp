#pragma once

// the first step of a recursive subproblem
// given the problem state, n, p, k, etc.
// we will randomly select a QuadDag from the pool
// the QuadDag selector contains 2 selectors
// 1. Remainder QuadDag selector, which works when n <= 4
// 2. Union QuadDag selector, which works when n > 4

#include "quad_dag_selector_remainder.hpp"
#include "quad_dag_selector_union.hpp"

namespace flowbench {

class QuadDagSelector : public Singleton<QuadDagSelector> {
public:
    QuadDagSelector() = default;
    uint32_t select(const ProblemState& state) const;
};

uint32_t QuadDagSelector::select(const ProblemState& state) const {
    if (state.n <= QD_VERTEX_CNT) {
        return RemainderQuadDagSelector::getInstance().select(state);
    } else {
        return UnionQuadDagSelector::getInstance().select(state);
    }
}

}