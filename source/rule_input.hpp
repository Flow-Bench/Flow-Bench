#pragma once

// how to input the rules
// we support 2 styles of rule input
//        and 3 types of fields (EM, LPM, and RM)

#include "rule_input_parser.hpp"
#include "rule_format.hpp"
#include "rule.hpp"

namespace flowbench {

template <class T>
void EmField<T>::load(std::istream& is) {
    static std::string str;
    is >> str;
    if (str == "R") {
        is >> str;
        RuleFormat::inputFormat.setStyle(RuleInputStyle::FlowBench);
    } else if (str[0] == '@') {
        str = str.substr(1);
        RuleFormat::inputFormat.setStyle(RuleInputStyle::ClassBench);
    }
    uint8_t width = RuleFormat::inputFormat.getWidth();
    auto style = RuleFormat::inputFormat.getStyle();
    if (style == RuleInputStyle::FlowBench) {
        if (str == "*") {
            value = getZeroOf<T>();
            wildcard = true;
        } else {
            value = parseHexString<T>(str, width);
            wildcard = false;
        }
    } else if (style == RuleInputStyle::ClassBench) {
        auto index = str.find('/');
        if (index == std::string::npos) {
            throw std::invalid_argument("invalid classbench rule");
        }
        value = parseHexString<T>(str.substr(0, index), width);
        T mask = parseHexString<T>(str.substr(index + 1), width);
        wildcard = mask == getZeroOf<T>();
    }
}

template <class T>
void LpmField<T>::load(std::istream& is) {
    static std::string str;
    is >> str;
    if (str == "R") {
        is >> str;
        RuleFormat::inputFormat.setStyle(RuleInputStyle::FlowBench);
    } else if (str[0] == '@') {
        str = str.substr(1);
        RuleFormat::inputFormat.setStyle(RuleInputStyle::ClassBench);
    }
    uint8_t width = RuleFormat::inputFormat.getWidth();
    auto style = RuleFormat::inputFormat.getStyle();
    if (style == RuleInputStyle::FlowBench) {
        if (str == "*") {
            prefix = getZeroOf<T>();
            prefixLength = 0;
        } else {
            prefix = parseBinaryString<T>(str);
            prefixLength = str.length();
        }
    } else if (style == RuleInputStyle::ClassBench) {
        auto index = str.find('/');
        if (index == std::string::npos) {
            throw std::invalid_argument("invalid classbench rule");
        }
        prefixLength = std::stoi(str.substr(index + 1));
        if (width == 32) {
            // for IPv4, we need to convert the prefix length to the number of bytes
            auto dotIndex = str.find('.');
            uint32_t prefixValue = 0;
            while (dotIndex != std::string::npos) {
                prefixValue = (prefixValue << 8) + std::stoi(str.substr(0, dotIndex));
                str = str.substr(dotIndex + 1);
                dotIndex = str.find('.');
            }
            prefixValue = (prefixValue << 8) + std::stoi(str.substr(0, index));
            prefix = T(prefixValue);
        } else {
            prefix = parseHexString<T>(str.substr(0, index), width);
        }
    }
}

template <class T>
void RmField<T>::load(std::istream& is) {
    static std::string min, _, max;
    is >> min;
    if (min[0] == 'R') {
        is >> min;
        RuleFormat::inputFormat.setStyle(RuleInputStyle::FlowBench);
    } else if (min[0] == '@') {
        min = min.substr(1);
        RuleFormat::inputFormat.setStyle(RuleInputStyle::ClassBench);
    }
    is >> _ >> max;
    uint8_t width = RuleFormat::inputFormat.getWidth();
    start = parseDecimalString<T>(min, width);
    end = parseDecimalString<T>(max, width);
}

std::istream& operator>>(std::istream& is, MatchField& field) {
    field.load(is);
    return is;
}

template <class T> // where T : RuleType
std::istream& operator>>(std::istream& is, Rule<T>& rule) {
    for (uint8_t i = 0; i < rule.getFieldCount(); i++) {
        RuleFormat::inputFormat.setWidth(rule.getRuleType().getFieldWidth(i));
        rule.getField(i).load(is);
    }
    return is;
}

}