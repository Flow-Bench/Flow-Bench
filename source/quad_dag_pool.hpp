#pragma once

// the QuadDag pool
// store all profiles for all valid QuadDags

#include "quad_dag_profile.hpp"

namespace flowbench {

class QuadDagPool : public std::vector<std::unique_ptr<QuadDagProfile>>, public Singleton<QuadDagPool> {
public:
    QuadDagPool() = default;
    QuadDagPool(std::istream& is) {
        std::string temp;
        while (is >> temp, temp == "DAG") {
            auto profile = std::make_unique<QuadDagProfile>(is);
            push_back(std::move(profile));
        }
    }

    const QuadDagProfile& getProfile(uint32_t id) const {
        return *at(id);
    }
};

}