#pragma once

// a enumarate match type class
// support EM, LPM, RM

#include "enumrate.hpp"

namespace flowbench {

enum class MatchType {
    Unknown,
    EM, // Exact match
    LPM, // Longest prefix match
    RM, // Range match
};

template <>
const auto getNames<MatchType>() {
    static const std::unordered_map<MatchType, const char*> names = {
        {MatchType::EM, "EM"},
        {MatchType::LPM, "LPM"},
        {MatchType::RM, "RM"},
    };
    return &names;
}

}