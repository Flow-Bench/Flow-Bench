#pragma once

// the second step of the trace generation
// we will allocate n traces to some flows according to the Pareto distribution
// we will allocate n traces to some rules according to the Pareto distribution as well
// and we will allocate the flows to the rules greedily

#include "pareto_allocator.hpp"
#include "configuration_trace.hpp"

namespace flowbench {

class TraceAllocator : public Singleton<TraceAllocator> {
private:
    std::vector<uint32_t> ruleAllocation;
    std::vector<uint32_t> flowAllocation;
    std::vector<std::vector<uint32_t>> ruleFlowAllocation;

public:
    TraceAllocator() = default;

    std::vector<std::vector<uint32_t>>& operator()(uint32_t traceCount, uint32_t ruleCount);
};

std::vector<std::vector<uint32_t>>& TraceAllocator::operator()(uint32_t traceCount, uint32_t ruleCount) {
    ParetoAllocator& paretoAllocator = ParetoAllocator::getInstance();
    if (TraceConfiguration::getInstance().enableFastMode()) {
        flowAllocation = paretoAllocator.allocate(traceCount, UINT32_MAX, ParetoDistribution::flowDistribution);
        ruleFlowAllocation.clear();
        for (uint32_t i = 0; i < flowAllocation.size(); i++) {
            uint32_t ruleIndex = Random::getInstance().nextInt32(0, ruleCount - 1);
            if (ruleFlowAllocation.size() <= ruleIndex) {
                std::vector<uint32_t> allocation { flowAllocation[i] };
                ruleFlowAllocation.push_back(allocation);
            } else {
                ruleFlowAllocation[ruleIndex].push_back(flowAllocation[i]);
            }
        }
    } else {    
        ruleAllocation = paretoAllocator.allocate(traceCount, ruleCount, ParetoDistribution::ruleDistribution);
        ruleFlowAllocation.clear();
        ruleFlowAllocation.resize(ruleAllocation.size());
        for (uint32_t i = 0; i < ruleAllocation.size(); i++) {
            ruleFlowAllocation[i] = paretoAllocator.allocate(ruleAllocation[i], UINT32_MAX, ParetoDistribution::flowDistribution);
        }
    }
    std::sort(ruleFlowAllocation.begin(), ruleFlowAllocation.end(), [](const std::vector<uint32_t>& a, const std::vector<uint32_t>& b) {
        return a.size() > b.size();
    });
    return ruleFlowAllocation;
}

}