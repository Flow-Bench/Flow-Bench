#pragma once

// the union QuadDag selector
// it works when n > 4
// which means we select a QuadDag whose 4 solid rules are all in the result set
// and we select some of the virtual rules as the parents of the next layer's search

#include "exception.hpp"
#include "divider_manager.hpp"
#include "normal_distribution.hpp"
#include "random_selector.hpp"
#include "problem_state.hpp"
#include "quad_dag_pool.hpp"
#include "parameter_calculator.hpp"

namespace flowbench {

class UnionQuadDagSelector : public Singleton<UnionQuadDagSelector> {
private:
    // we use 2 look-up tables to store the QuadDags
    // lut[k][p1][Mp2][mp2] means : the QuadDags satisfying:
    //                              the 4 solid rules has no more than k+1 fields
    //                              intra-layer parameter is p1
    //                              maximum of inter-layer parameter >= Mp2 + 1
    //                              minimum of inter-layer parameter <= mp2
    // and lutnw is the version where wildcard is not allowed
    std::array<std::array<std::array<std::array<std::vector<uint32_t>, QD_VERTEX_CNT>, QD_VERTEX_CNT>, QD_VPAIR_CNT+1>, QD_FIELD_CNT> lut;
    std::array<std::array<std::array<std::array<std::vector<uint32_t>, QD_VERTEX_CNT>, QD_VERTEX_CNT>, QD_VPAIR_CNT+1>, QD_FIELD_CNT> lutnw;

    // weights for selection
    std::array<double, QD_VPAIR_CNT+1> weights;

    // tables for selection
    std::array<std::vector<uint32_t>*, QD_VPAIR_CNT+1> tables;

private:
    // we use a normal distribution to select the QuadDag
    constexpr static double mean = 0.0;
    constexpr static double variance = (QD_VPAIR_CNT / 2.0) * (QD_VPAIR_CNT / 2.0);
    const static NormalDistribution dist;

public:
    UnionQuadDagSelector();
    uint32_t select(const ProblemState& state);
};

const NormalDistribution UnionQuadDagSelector::dist(mean, variance);

UnionQuadDagSelector::UnionQuadDagSelector() {
    const auto& pool = QuadDagPool::getInstance();
    for (uint32_t i = 0; i < pool.size(); i++) {
        const auto& profile = pool.getProfile(i);
        const auto& virtualRules = profile.getVirtualRules();
        uint32_t p1 = profile.getTotalParameter();
        for (uint32_t k = profile.getActualFieldCount() - 1; k < QD_FIELD_CNT; k++) {
            for (uint32_t Mp2 = 0; Mp2 < virtualRules.getMaxParameter(); Mp2++) {
                for (uint32_t mp2 = virtualRules.getMinParameter(); mp2 < QD_VERTEX_CNT; mp2++) {
                    lut[k][p1][Mp2][mp2].push_back(i);
                    if (!profile.getExistWildcard()) {
                        lutnw[k][p1][Mp2][mp2].push_back(i);
                    }
                }
            }
        }
    }
}

uint32_t UnionQuadDagSelector::select(const ProblemState& state) {
    uint8_t k = std::min(state.k, QD_FIELD_CNT);
    uint32_t p = state.p;
    uint32_t n = state.n;
    const auto& divider = DividerManager::getInstance().getDivider(n - QD_VERTEX_CNT);
    uint32_t sumOfMaxParameters = 0;
    for (uint32_t i = 0; i < QD_VERTEX_CNT; i++) {
        sumOfMaxParameters += ParameterCalculator::getInstance().at(divider.result[i]);
    }
    uint32_t maxP1 = std::min<uint32_t>(QD_VPAIR_CNT, p);
    uint32_t minP1 = [&]() {
        uint32_t temp = QD_VERTEX_CNT * (n - QD_VERTEX_CNT) + sumOfMaxParameters;
        return p < temp ? 0 : p - temp;
    }();
    double alpha1 = static_cast<double>(QD_VPAIR_CNT) * p / ParameterCalculator::getInstance().at(n);
    std::fill(weights.begin(), weights.end(), 0.0);
    for (uint32_t p1 = minP1; p1 <= maxP1; p1++) {
        uint32_t minMaxP2 = [&]() {
            int32_t temp = p;
            temp -= p1 + sumOfMaxParameters;
            return std::max(1.0, std::ceil(1.0 / (n - QD_VERTEX_CNT) * temp));
        }();
        uint32_t maxMinP2 = std::min(QD_VERTEX_CNT - 1.0, std::floor((p - p1) / (n - QD_VERTEX_CNT)));
        if (state.allowWildcard) {
            tables[p1] = &lut[k-1][p1][minMaxP2-1][maxMinP2];
        } else {
            tables[p1] = &lutnw[k-1][p1][minMaxP2-1][maxMinP2];
        }
        if (!tables[p1]->empty()) {
            weights[p1] = dist.getProbability(p1 - alpha1);
        }
    }
    uint32_t p1 = RandomSelector::getInstance().select(weights);
    const auto& table = *tables[p1];
    uint32_t index = Random::getInstance().nextInt32(0, table.size() - 1);
    return table[index];
}

}