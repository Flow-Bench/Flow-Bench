#include <fstream>
#include <iostream>

#include "quad_dag_generator.hpp"
#include "quad_dag_instantiater_normal.hpp"
#include "quad_dag_instantiater_dense.hpp"
#include "time_report.hpp"

int main(int argc, char** argv) {
    double time = flowbench::reportTime([&]() {
        std::ofstream osn(flowbench::NORMAL_PROFILE_PATH);
        flowbench::QuadDagGenerator<flowbench::NormalQuadDagInstantiater>::getInstance()(osn);
        std::ofstream osd(flowbench::DENSE_PROFILE_PATH);
        flowbench::QuadDagGenerator<flowbench::DenseQuadDagInstantiater>::getInstance()(osd);
    });
    std::cout << "Time: " << time << "s" << std::endl;
    return 0;
}