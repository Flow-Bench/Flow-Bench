#pragma once

// solve a global problem

#include "time_report.hpp"
#include "partition_dense.hpp"
#include "partition_sparse.hpp"
#include "problem_local.hpp"
#include "rule_output.hpp"

namespace flowbench {

class GlobalProblem {
private:
    // the origins of the sub-problems
    std::queue<std::unique_ptr<ProblemState>> subProblems;
    // the state queue in a sub-problem
    std::queue<std::unique_ptr<ProblemState>> stateQueue;
    UDRuleSet finalSet;
    double time;
    bool solved = false;

public:
    GlobalProblem() = default;

    // solve the global problem
    // default: start from wildcard, if failed:
    // 1. if the problem is sparse, use the sparse partition
    // 2. if the problem is dense, use the dense partition
    // until the problem is solved or the partition fails
    bool solve();

    void print(std::ostream& os) const;
    void report(std::ostream& os) const;

private:
    // initialize the global problem
    bool initialize() {
        while (!subProblems.empty()) {
            subProblems.pop();
        }
        while (!stateQueue.empty()) {
            stateQueue.pop();
        }
        finalSet.clear();
    }

    // solve a sub-problem (the front of the sub-problems queue)
    bool solveSubProblem();

    // solve all sub-problems
    bool solveAllSubProblems();

};

bool GlobalProblem::solve() {
    int n = Configuration::getInstance().getRuleCount();
    int p = Task::getInstance().getValue();
    time = 0;
    Partition* partition = &SparsePartition::getInstance();
    if (p > ParameterCalculator::getInstance().at(n)) {
        partition = &DensePartition::getInstance();
    }
    do {
        initialize();
        partition->exportOrigins(finalSet, subProblems);
        if (solveAllSubProblems()) {
            solved = true;
            return true;
        }
    } while (partition->addPartition());
    return false;
}

bool GlobalProblem::solveSubProblem() {
    auto subProblem = std::move(subProblems.front());
    subProblems.pop();
    bool success = true;
    while (!stateQueue.empty()) {
        stateQueue.pop();
    }
    time += reportTime([&]() {
        stateQueue.push(std::move(subProblem));
        while (!stateQueue.empty()) {
            auto state = std::move(stateQueue.front());
            stateQueue.pop();
            if (!LocalProblem::getInstance().solve(std::move(state))) {
                success = false;
                break;
            } else {
                LocalProblem::getInstance().exportRules(finalSet, stateQueue);
            }
        }
    });
    return success;
}

bool GlobalProblem::solveAllSubProblems() {
    bool success = true;
    while (!subProblems.empty()) {
        if (!solveSubProblem()) {
            success = false;
            break;
        }
    }
    return success;
}

void GlobalProblem::print(std::ostream& os) const {
    for (int i = 0; i < finalSet.size(); i++) {
        os << finalSet.getRule(i) << '\n';
    }
}

void GlobalProblem::report(std::ostream& os) const {
    os << "Total time: " << time << "s\n";
    Configuration::getInstance().print(os);
    if (solved) {
        os << "Solved.\n";
    } else {
        os << "Failed.\n";
    }
}


}