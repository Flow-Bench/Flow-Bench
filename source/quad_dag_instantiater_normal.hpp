#pragma once

// the normal strategy
// we will always add an extra bit to the field
// for example:
// XX0 -> XX10, XX100, XX1000, etc.
// that is, every field will end in 0 (or wildcard)
// it will never happen that both XX0 and XX1 exist in the same candidate rule set

// this guarantees that every rule can be hit by some flows
// otherwise, e.g. the rule set *, 0, 1, we will find that * can be hit by no flow
// because 0 and 1 are more specific and they have covered all the match space

#include "singleton.hpp"
#include "quad_dag_instantiater.hpp"

namespace flowbench {

class NormalQuadDagInstantiater : public QuadDagInstantiater, public Singleton<NormalQuadDagInstantiater> {
protected:
    uint8_t nextBitWidth(uint8_t bitWidth) const override {
        return bitWidth + 1;
    }
};

}