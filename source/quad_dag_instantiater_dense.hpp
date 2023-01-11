#pragma once

// the dense strategy
// no extra bit will be added
// for example:
// XX0 -> XX1, XX10, XX100, XX1000, etc.

// this strategy will use fewer bits to represent the rule set
// when you find the bits are not enough, you can switch to dense mode
// however, some rules in dense mode may not be hit by any flow
// so we recommend to use normal mode if possible

#include "singleton.hpp"
#include "quad_dag_instantiater.hpp"

namespace flowbench {

class DenseQuadDagInstantiater : public QuadDagInstantiater, public Singleton<DenseQuadDagInstantiater> {
protected:
    uint8_t nextBitWidth(uint8_t bitWidth) const override {
        return bitWidth;
    }
};

}