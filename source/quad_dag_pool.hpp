#pragma once

// the QuadDag pool
// store all profiles for all valid QuadDags

#include "quad_dag_profile.hpp"

namespace flowbench {

class QuadDagPool : public std::vector<QuadDagProfile>, public Singleton<QuadDagPool> {
public:
    QuadDagPool() = default;
    QuadDagPool(std::istream& is) {
        std::string temp;
        while (is >> temp, temp != "EOF") {
            emplace_back(is);
        }
    }

    const QuadDagProfile& getProfile(uint32_t id) const {
        return at(id);
    }
};

}