#pragma once

// specify the style and width of rule format

#include "rule_output_style.hpp"

namespace flowbench {

class RuleFormat {
private:
    uint8_t width = 0;
    RuleOutputStyle style = RuleOutputStyle::FlowBench;

public:
    RuleFormat() = default;
    uint8_t getWidth() const {
        return width;
    }

    RuleOutputStyle getStyle() const {
        return style;
    }

    void setWidth(uint8_t width) {
        this->width = width;
    }

    void setStyle(RuleOutputStyle style) {
        this->style = style;
    }

    static RuleFormat inputFormat;
    static RuleFormat outputFormat;
};

RuleFormat RuleFormat::inputFormat;
RuleFormat RuleFormat::outputFormat;

}