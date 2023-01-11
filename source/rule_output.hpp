#pragma once

// how to output the rules
// we support 2 styles of rule output
//        and 3 types of fields (EM, LPM, and RM)

#include <iomanip>

#include "rule.hpp"
#include "rule_output_style.hpp"
#include "rule_format.hpp"
#include "configuration.hpp"

namespace flowbench {

// for pre-computed files, we use FlowBench's default style (start from R)
// we will add a prefix (S or V) to the rule string, representing solid and virtual rules
template <>
RuleOutputStyle getStyle<RuleTypeCandidate>() {
    return RuleOutputStyle::FlowBench;
}

template <>
RuleOutputStyle getStyle<RuleTypeUD>() {
    return Configuration::getInstance().getOutputStyle();
}

template <class T>
void EmField<T>::print(std::ostream& os) const {
    uint8_t width = RuleFormat::outputFormat.getWidth();
    auto style = RuleFormat::outputFormat.getStyle();
    if (style == RuleOutputStyle::FlowBench) {
        os << std::setiosflags(std::ios::left) << std::setw((width + 3) / 4 + 3);
        if (isWildcard()) {
            os << "*";
        } else {
            os << ("0x" + getValue().toHexString(width));
        }
    } else if (style == RuleOutputStyle::ClassBench) {
        os << "0x" << getValue().toHexString(width) << "/";
        if (isWildcard()) {
            os << "0x" << getZeroOf<T>().toHexString(width);
        } else {
            os << "0x" << getMaxOf<T>().toHexString(width);
        }
    }
}

template <class T>
void LpmField<T>::print(std::ostream& os) const {
    uint8_t width = RuleFormat::outputFormat.getWidth();
    auto style = RuleFormat::outputFormat.getStyle();
    if (style == RuleOutputStyle::FlowBench) {
        os << std::setiosflags(std::ios::left) << std::setw(width + 1);
        if (isWildcard()) {
            os << "*";
        } else {
            os << getPrefix().toBinaryString(getPrefixLength());
        }
    } else if (style == RuleOutputStyle::ClassBench) {
        if (width == 32) {
            // special case: 32 bits (IPv4 address) 0.0.0.0/0
            std::string seg1 = getPrefix().toDecimalString(8);
            std::string seg2 = (getPrefix() << 8).toDecimalString(8);
            std::string seg3 = (getPrefix() << 16).toDecimalString(8);
            std::string seg4 = (getPrefix() << 24).toDecimalString(8);
            os << seg1 << "." << seg2 << "." << seg3 << "." << seg4 << "/" << (int)getPrefixLength();
        } else {
            // general case: 0x00000000/0
            os << "0x" << getPrefix().toHexString(width) << "/" << (int)getPrefixLength();
        }
    }
}

template <class T>
void RmField<T>::print(std::ostream& os) const {
    uint8_t width = RuleFormat::outputFormat.getWidth();
    auto style = RuleFormat::outputFormat.getStyle();
    if (style == RuleOutputStyle::FlowBench) {
        os << std::setiosflags(std::ios::left) << std::setw((width + 1) / 3 + 1) << getMin().toDecimalString(width);
        os << " : ";
        os << std::setiosflags(std::ios::left) << std::setw((width + 1) / 3 + 1) << getMax().toDecimalString(width);
    } else if (style == RuleOutputStyle::ClassBench) {
        os << getMin().toDecimalString(width);
        os << " : ";
        os << getMax().toDecimalString(width);
    }
}

std::ostream& operator<<(std::ostream& os, const MatchField& field) {
    field.print(os);
    return os;
}

// print the rule in the specified style to the output stream
template <class T> // where T : RuleType
std::ostream& operator<<(std::ostream& os, const Rule<T>& rule) {
    RuleFormat::outputFormat.setStyle(getStyle<T>());
    auto style = RuleFormat::outputFormat.getStyle();
    if (style == RuleOutputStyle::FlowBench) {
        os << "R ";
    } else if (style == RuleOutputStyle::ClassBench) {
        os << "@";
    }
    for (uint8_t i = 0; i < rule.getFieldCount(); i++) {
        if (i > 0) {
            os << " ";
        }
        RuleFormat::outputFormat.setWidth(rule.getRuleType().getFieldWidth(i));
        rule.getField(i).print(os);
    }
    return os;
}

}