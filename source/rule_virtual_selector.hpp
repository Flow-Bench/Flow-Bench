#pragma once

// the second step of a recursive subproblem
// in the first step (QuadDag select), we have selected a QuadDag
// for n <= 4, we only need to put the first n solid rules into the result set
// for n > 4, we need to select some (<=4) of the virtual rules as the parents of the next layer's search
// this is what this selector does

#include "random_selector.hpp"
#include "quad_dag_profile.hpp"
#include "normal_distribution.hpp"
#include "divider.hpp"
#include "problem_state.hpp"
#include "parameter_calculator.hpp"

namespace flowbench {

class VirtualRuleSelector : public Singleton<VirtualRuleSelector> {
private:
    // we use a normal distribution to select the virtual rules
    constexpr static double mean = 0.0;
    constexpr static double variance = (QD_VERTEX_CNT / 2.0) * (QD_VERTEX_CNT / 2.0);
    const static NormalDistribution dist;

public:
    VirtualRuleSelector() = default;

    // the selected virtual rules, the index of the virtual rules are stored in the result set
    std::vector<uint8_t> result;

    // the parameters corresponding to the virtual rules (for next layer's search)
    std::vector<uint8_t> parameters;

    // select the virtual rules
    // n : state.n on the current layer
    // p : state.p on the current layer, will be updated to the sum of parameters on next layer
    // p1 : the intra-layer parameter of the selected QuadDag
    // profile : the profile of the selected QuadDag
    void select(ProblemState& state, const QuadDagProfile& profile);

};

const NormalDistribution VirtualRuleSelector::dist(mean, variance);

void VirtualRuleSelector::select(ProblemState& state, const QuadDagProfile& profile) {
    uint32_t n = state.n;
    uint8_t p = state.p;
    uint8_t p1 = profile.getTotalParameter();
    result.clear();
    parameters.clear();
    Divider divider(n - QD_VERTEX_CNT);
    uint32_t sumOfMaxParameters = 0;
    for (uint32_t i = 0; i < QD_VERTEX_CNT; i++) {
        sumOfMaxParameters += ParameterCalculator::getInstance().at(divider.result[i]);
    }
    double alpha2 = static_cast<double>(QD_VERTEX_CNT) * p / ParameterCalculator::getInstance().at(n);
    p -= p1;
    uint8_t Mp2 = profile.getVirtualRules().getMaxParameter();
    uint8_t mp2 = profile.getVirtualRules().getMinParameter();
    uint8_t candidateCount = profile.getVirtualRules().size();
    std::vector<double> weights(candidateCount);
    for (uint8_t i = 0; i < QD_VERTEX_CNT; i++) {
        if (divider.result[i] == 0) {
            continue;
        }
        uint8_t maxP2 = [&]() {
            double temp = p;
            temp -= mp2 * std::accumulate(divider.result.begin() + i + 1, divider.result.end(), 0);
            return std::min(static_cast<double>(QD_VERTEX_CNT), std::floor(temp / divider.result[i]));
        }();
        uint8_t minP2 = [&]() {
            double temp = p;
            temp -= sumOfMaxParameters + Mp2 * std::accumulate(divider.result.begin() + i + 1, divider.result.end(), 0);
            return std::max(0.0, std::ceil(temp / divider.result[i]));
        }();
        for (uint8_t j = 0; j < candidateCount; j++) {
            uint8_t p2 = profile.getVirtualRules().getParameter(j);
            if (p2 >= minP2 && p2 <= maxP2) {
                weights[j] += dist.getProbability(p2 - alpha2);
            }
        }
        uint8_t index = RandomSelector::getInstance().select(weights);
        result.push_back(index);
        p -= divider.result[i] * profile.getVirtualRules().getParameter(index);
    }
    if (sumOfMaxParameters > 0) {
        double ratio = static_cast<double>(p) / sumOfMaxParameters;
        for (uint8_t i = 0; i < QD_VERTEX_CNT; i++) {
            if (divider.result[i] != 0) {
                parameters.push_back(ParameterCalculator::getInstance().at(divider.result[i]) * ratio);
            }
        }
        int32_t diff = p;
        diff -= std::accumulate(parameters.begin(), parameters.end(), 0);
        while (diff > 0) {
            for (uint8_t i = 0; i < parameters.size() && diff > 0; i++) {
                if (parameters[i] < ParameterCalculator::getInstance().at(divider.result[i])) {
                    parameters[i]++;
                    diff--;
                }
            }
        }
        while (diff < 0) {
            for (uint8_t i = 0; i < parameters.size() && diff < 0; i++) {
                if (parameters[i] > 0) {
                    parameters[i]--;
                    diff++;
                }
            }
        }
    } else {
        parameters.resize(result.size(), 0);
    }
}

}