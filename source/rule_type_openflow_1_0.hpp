#pragma once

#include "rule_type.hpp"

namespace flowbench {

class RuleTypeOpenFlow1_0 : public RuleType, public Singleton<RuleTypeOpenFlow1_0> {
public:
    // OpenFlow 1.0 12-tuple
    uint8_t getFieldCount() const override {
        return 12;
    }

    // in_port, dl_src, dl_dst, dl_vlan, dl_vlan_pcp, dl_type
    // nw_tos, nw_proto, nw_src, nw_dst, tp_src, tp_dst
    uint8_t getFieldWidth(uint8_t fieldIndex) const override {
        switch (fieldIndex) {
            case 0:
                return 16;
            case 1:
                return 48;
            case 2:
                return 48;
            case 3:
                return 16;
            case 4:
                return 8;
            case 5:
                return 16;
            case 6:
                return 8;
            case 7:
                return 8;
            case 8:
                return 32;
            case 9:
                return 32;
            case 10:
                return 16;
            case 11:
                return 16;
            default:
                return 0;
        }
    }

    MatchType getMatchType(uint8_t fieldIndex) const override {
        switch (fieldIndex) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                return MatchType::EM;
            case 8:
            case 9:
                return MatchType::LPM;
            case 10:
            case 11:
                return MatchType::RM;
            default:
                return MatchType::EM;
        }
    }

    RuleTypeOpenFlow1_0() = default;
};

}