#pragma once

// the fourth step of the trace generation
// in the first step (Rule Isolation), we have splitted the rule set into isolate rules
// in the second step (Trace Allocation), we have allocated the traces according to the Pareto distribution
// in the third step (Rule Mapping), we have built a mapping between the allocation result and the isolate rule set
// in this step, we will build a mapping between the allocation result and the flows

// for each rule in the rule set, we have known the number of flows it need to hold
// 1. select the available fields
// 2. randomly select an available field and split it into two fields
// 3. repeat step 2 until the number of rules is satisfied
// 4. for all those rules, randomly generate a flow (exact rule)

#include <queue>
#include <algorithm>

#include "exception.hpp"
#include "trace.hpp"
#include "rule_set.hpp"
#include "rule_splitter.hpp"

namespace flowbench {

class FlowMapping : public Singleton<FlowMapping> {
private:
    Trace trace;

public:
    const Trace& operator()(UDRuleSet& ruleSet, const std::vector<std::vector<uint32_t>>& ruleFlowAllocation);

private:
    void generateFlows(std::unique_ptr<UDRule> rule, const std::vector<uint32_t>& flowAllocation);
};

const Trace& FlowMapping::operator()(UDRuleSet& ruleSet, const std::vector<std::vector<uint32_t>>& ruleFlowAllocation) {
    trace.clear();
    for (uint32_t i = 0; i < ruleSet.size(); i++) {
        if (ruleSet[i] != nullptr) {
            generateFlows(std::move(ruleSet[i]), ruleFlowAllocation[i]);   
        }
    }
    std::random_shuffle(trace.begin(), trace.end());
    return trace;
}

void FlowMapping::generateFlows(std::unique_ptr<UDRule> rule, const std::vector<uint32_t>& flowAllocation) {
    uint32_t flowCount = flowAllocation.size();
    std::queue<std::unique_ptr<UDRule>> rules;
    rules.push(std::move(rule));
    while (rules.size() < flowCount) {
        auto rule = std::move(rules.front());
        rules.pop();
        auto pair = RuleSplitter::getInstance().split(*rule);
        if (pair.first != nullptr) {
            rules.push(std::move(pair.first));
        } else {
            throw NoRuleError();
        }
        if (pair.second != nullptr) {
            rules.push(std::move(pair.second));
        }
    }
    uint32_t flowIndex = 0;
    while (!rules.empty()) {
        auto rule = std::move(rules.front());
        rules.pop();
        Flow flow(*rule);
        // duplicate the flow according to the flow allocation
        for (uint32_t i = 0; i < flowAllocation[flowIndex]; i++) {
            trace.push_back(flow.clone());
        }
        flowIndex++;
    }
}

}