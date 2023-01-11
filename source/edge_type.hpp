#pragma once

#include "enumrate.hpp"

namespace flowbench {

enum class EdgeType {
    Unknown,
    None,
    Overlap,
    Cover
};

template <>
const auto getNames<EdgeType>() {
    static const std::unordered_map<EdgeType, const char*> names = {
        {EdgeType::None, "None"},
        {EdgeType::Overlap, "Overlap"},
        {EdgeType::Cover, "Cover"}
    };
    return &names;
}

}