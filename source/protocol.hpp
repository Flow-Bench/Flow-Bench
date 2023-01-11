#pragma once

// pre-defined protocols

#include "enumrate.hpp"

namespace flowbench {

enum class Protocol {
    Unknown,
    IPv4,
    IPv6,
    OpenFlow1_0
};

template <>
const auto getNames<Protocol>() {
    static const std::unordered_map<Protocol, const char*> names = {
        {Protocol::IPv4, "ipv4"},
        {Protocol::IPv6, "ipv6"},
        {Protocol::OpenFlow1_0, "openflow1.0"}
    };
    return &names;
}

}