// the rule set generator
// main source file

#include <fstream>

#include "configuration.hpp"
#include "quad_dag_pool.hpp"
#include "rule_input.hpp"
#include "rule_set_generator.hpp"

int main(int argc, char* argv[]) {
    flowbench::Configuration::setInstance(argc, argv);
    std::ifstream is(flowbench::Configuration::getInstance().getQuadDagFilePath());
    flowbench::QuadDagPool::setInstance(is);
    is.close();
    std::ofstream os(flowbench::Configuration::getInstance().getOutputFilePath());
    flowbench::RuleSetGenerator::getInstance()(os);
    os.close();
    flowbench::TimeRecorder::getInstance().report(std::cout);
    flowbench::RuleSetGenerator::getInstance().report(std::cout);
    return 0;
}