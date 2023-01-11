#pragma once

// the base class of partition algorithm

// for most cases, FlowBench will start from the same origin: wildcard
// however, if what the user want is very sparse or very dense, the bit width may be insufficient
// to partially solve this problem, we design a partition algorithm to divide the problem into several sub-problems
// e.g. if the user wants a lot of rules with NO dependency
//      in basic algorithm, we will select the DAG NNNNNN (3 bits) in the normal mode
//      but we will split the virtual rule, which may introduce another 1-2 bits
//      in result, a layer of the tree will use up to 5 bits, but it just provides 4x solid rules
//      if the rule count is set very large, the tree will be very deep, and the bit width will be insufficient
//      to solve this problem, we can split the problem into several sub-problems
//      we do not start from wildcard, but split wildcard to a set of origins first
//      for example, 000, 001, 010, 011, 100, 101, 110, 111
//      in this case, we use 3 bits to produce 8x solid rules, much more than the basic algorithm

#include "configuration.hpp"
#include "task.hpp"

namespace flowbench {

class Partition {
protected:
    uint32_t n, p;      // n: rule count, p: parameter
    uint8_t totalWidth; // total bit width (LPM/RM)
    uint32_t partCount; // partition count

public:
    Partition() {
        n = Configuration::getInstance().getRuleCount();
        p = Task::getInstance().getValue();
        totalWidth = 0;
        partCount = 1;
        for (uint8_t i = 0; i < RuleTypeUD::getInstance().getFieldCount(); i++) {
            if (RuleTypeUD::getInstance().getMatchType(i) != MatchType::EM) {
                totalWidth += RuleTypeUD::getInstance().getFieldWidth(i);
            }
        }
    }

public:
    virtual bool addPartition() = 0;
    virtual bool exportOrigins(UDRuleSet& finalSet, std::queue<std::unique_ptr<ProblemState>> &origins) const = 0;

};

}