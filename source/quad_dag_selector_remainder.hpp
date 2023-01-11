#pragma once

// the remainder QuadDag selector
// it works when n <= 4
// which means we will select a incomplete QuadDag, and only part of (first n solid rules) are actually selected
// in this case, we focus on the partial attributes (D or E) of the QuadDags

#include "problem_state.hpp"
#include "quad_dag_pool.hpp"
#include "random.hpp"
#include "exception.hpp"

namespace flowbench {

class RemainderQuadDagSelector : public Singleton<RemainderQuadDagSelector> {
private:
    // we use 2 look-up tables to store the partial QuadDags
    // lut[n][k][p] means : the QuadDags satisfying:
    //                      the first n+1 rules has no more than k+1 fields
    //                      and has a parameter exactly p
    // and lutnw is the version where wildcard is not allowed
    std::array<std::array<std::array<std::vector<uint32_t>, QD_VPAIR_CNT+1>, QD_FIELD_CNT>, QD_VERTEX_CNT> lut;
    std::array<std::array<std::array<std::vector<uint32_t>, QD_VPAIR_CNT+1>, QD_FIELD_CNT>, QD_VERTEX_CNT> lutnw;

public:
    RemainderQuadDagSelector();
    uint32_t select(const ProblemState& state) const;
};

RemainderQuadDagSelector::RemainderQuadDagSelector() {
    for (uint32_t i = 0; i < QuadDagPool::getInstance().size(); i++) {
        const auto& profile = QuadDagPool::getInstance().getProfile(i);
        const auto& solidRules = profile.getSolidRules();
        for (uint32_t n = 0, p = 0; n < QD_VERTEX_CNT; n++) {
            p += solidRules.getParameter(n);
            for (uint32_t k = profile.getActualFieldCount() - 1; k < QD_FIELD_CNT; k++) {
                lut[n][k][p].push_back(i);
                if (!profile.getExistWildcard()) {
                    lutnw[n][k][p].push_back(i);
                }
            }
        }
    }
}

uint32_t RemainderQuadDagSelector::select(const ProblemState& state) const {
    uint8_t k = std::min(state.k, QD_FIELD_CNT);
    uint32_t n = state.n, p = state.p;
    const auto& table = state.allowWildcard ? lut : lutnw;
    const auto& candidates = table[n-1][k-1][p];
    if (candidates.empty()) {
        throw NoCandidateError();
    }
    auto index = Random::getInstance().nextInt32(0, candidates.size() - 1);
    return candidates[index];
}

}