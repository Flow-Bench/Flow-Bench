#pragma once

// we support 2 styles of rule output
// 1. FlowBench's default style
// 2. ClassBench style (for compatibility with ClassBench)

#include "enumrate.hpp"
#include "rule_type_candidate.hpp"
#include "rule_type_ud.hpp"

namespace flowbench {

enum class RuleOutputStyle {
    Unknown,
    FlowBench,
    ClassBench
};

template <>
const auto getNames<RuleOutputStyle>() {
    static const std::unordered_map<RuleOutputStyle, const char*> names = {
        {RuleOutputStyle::FlowBench, "FlowBench"},
        {RuleOutputStyle::ClassBench, "ClassBench"}
    };
    return &names;
}

template <class T> // where T : RuleType
RuleOutputStyle getStyle() {
    return RuleOutputStyle::Unknown;
}

using RuleInputStyle = RuleOutputStyle;

}