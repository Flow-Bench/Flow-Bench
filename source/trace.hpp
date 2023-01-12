#pragma once

// trace to be printed

#include "flow.hpp"
#include "rule_format.hpp"

namespace flowbench {

class Trace : public std::vector<std::unique_ptr<Flow>> {
public:
    Trace() = default;

    void print(std::ostream& os) const;
};

void Trace::print(std::ostream& os) const {
    auto style = RuleFormat::outputFormat.getStyle();
    if (style == RuleOutputStyle::FlowBench) {
        for (const auto& flow : *this) {
            for (uint8_t i = 0; i < RuleTypeUD::getInstance().getFieldCount(); i++) {
                os << "0x" << flow->getField(i).toHexString(RuleTypeUD::getInstance().getFieldWidth(i)) << " ";
            }
            os << " " << flow->getRuleIndex() << "\n";
        }
    } else if (style == RuleOutputStyle::ClassBench) {
        for (const auto& flow : *this) {
            for (uint8_t i = 0; i < RuleTypeUD::getInstance().getFieldCount(); i++) {
                os << flow->getField(i).toDecimalString(RuleTypeUD::getInstance().getFieldWidth(i));
                if (i < RuleTypeUD::getInstance().getFieldCount() - 1) {
                    os << ",";
                }
            }
            os << " " << flow->getRuleIndex() << "\n";
        }
    }
}

}