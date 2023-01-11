#pragma once

// users may specify one of the tasks for DAG generation
// or FlowBench randomly generate a task if not specified

#include "enumrate.hpp"

namespace flowbench {

enum class TaskType {
    Unknown,
    DependencyLength,
    EdgeCount
};

template <>
const auto getNames<TaskType>() {
    static const std::unordered_map<TaskType, const char*> names = {
        {TaskType::DependencyLength, "DependencyLength"},
        {TaskType::EdgeCount, "EdgeCount"}
    };
    return &names;
}

}