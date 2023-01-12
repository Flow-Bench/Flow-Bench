#include <fstream>
#include <iostream>

#include "rule_pool.hpp"
#include "trace_generator.hpp"
#include "time_report.hpp"

int main(int argc, char** argv) {
    flowbench::TraceConfiguration::setInstance(argc, argv);
    std::ifstream is(flowbench::TraceConfiguration::getInstance().getInputFilePath());
    flowbench::RulePool::setInstance(is);
    is.close();
    flowbench::TraceConfiguration::getInstance().print(std::cout);
    std::ofstream os(flowbench::TraceConfiguration::getInstance().getOutputFilePath());
    double time = flowbench::reportTime([&]() {
        flowbench::TraceGenerator::getInstance().generate(os);
    });
    os.close();
    std::cout << "Time: " << time << "s" << std::endl;
    return 0;
}