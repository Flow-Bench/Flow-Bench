#pragma once

// constants

#include <cstdint>

namespace flowbench {

// QuadDag attributes
constexpr uint8_t QD_VERTEX_CNT = 4;
constexpr uint8_t QD_VPAIR_CNT  = 6;
constexpr uint8_t QD_VTRIAN_CNT = 4;

constexpr uint8_t QD_FIELD_CNT  = 3;

// QuadDag profile paths
constexpr const char* NORMAL_PROFILE_PATH = "normal_profile.txt";
constexpr const char* DENSE_PROFILE_PATH = "dense_profile.txt";

// default configuration
constexpr uint32_t DEFAULT_RULE_CNT = 4096;

}