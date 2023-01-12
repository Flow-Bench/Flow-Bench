#pragma once

// the main class of trace generator
// 1. Rule Isolation, split the rule set into isolate rules
// 2. Trace Allocation, allocate the traces according to the Pareto distribution
// 3. Rule Mapping, build a mapping between the allocation result and the isolate rule set
// 4. Flow Mapping, build a mapping between the allocation result and the flows

#include "rule_isolator.hpp"
#include "trace_allocator.hpp"
#include "mapping_rule.hpp"
#include "mapping_flow.hpp"
#include "rule_output.hpp"

namespace flowbench {

class TraceGenerator : public Singleton<TraceGenerator> {
public:
    TraceGenerator() = default;
    void generate(std::ostream& os) const;
};

void TraceGenerator::generate(std::ostream& os) const {
    uint32_t traceCount = TraceConfiguration::getInstance().getTraceCount();
    uint32_t ruleCount = TraceConfiguration::getInstance().getRuleCount();
    auto ruleSet = RuleIsolator::getInstance()();
    auto& ruleFlowAllocation = TraceAllocator::getInstance()(traceCount, ruleCount);
    auto& rules = RuleMapping::getInstance()(*ruleSet, ruleFlowAllocation);
    const auto& trace = FlowMapping::getInstance()(rules, ruleFlowAllocation);
    trace.print(os);
}

}