#pragma once

// the rule set generator

#include "problem_global.hpp"

namespace flowbench {

class RuleSetGenerator : public Singleton<RuleSetGenerator> {
private:
    GlobalProblem global;

public:
    // generate all rules to the give ostream
    void operator()(std::ostream& os);

    // print the report to the give ostream
    void report(std::ostream& os) const;
};

void RuleSetGenerator::operator()(std::ostream& os) {
    if (global.solve()) {
        global.print(os);
    } else {
        os << "Failed to generate the rule set." << std::endl;
    }
}

void RuleSetGenerator::report(std::ostream& os) const {
    global.report(os);
}

}