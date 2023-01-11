#pragma once

// abstract sized field class
// fields -> sized field -> match field
// range: [min, max], min <= max

#include "integer.hpp"
#include "match_field.hpp"

namespace flowbench {

template <class T> // where T : Integer
class SizedField : public MatchField {
public:
    virtual T getMin() const = 0;
    virtual T getMax() const = 0;

    virtual bool operator==(const SizedField& other) const {
        return getMin() == other.getMin() && getMax() == other.getMax();
    }
    virtual bool operator!=(const SizedField& other) const {
        return !(*this == other);
    }
    virtual bool overlap(const SizedField& other) const {
        return getMin() <= other.getMax() && getMax() >= other.getMin();
    }
    virtual bool cover(const SizedField& other) const {
        return getMin() <= other.getMin() && getMax() >= other.getMax();
    }

    // we must guarantee the following functions are used between the same type
    virtual bool operator==(const MatchField& other) const override {
        return *this == static_cast<const SizedField&>(other);
    }
    virtual bool operator!=(const MatchField& other) const override {
        return !(*this == other);
    }
    virtual bool overlap(const MatchField& other) const override {
        return overlap(static_cast<const SizedField&>(other));
    }
    virtual bool cover(const MatchField& other) const override {
        return cover(static_cast<const SizedField&>(other));
    }

    virtual bool isWildcard() const {
        return getMin().isZero() && getMax().isMax();
    }

    virtual void addSuffix(const T& suffix, uint8_t suffixLength) {}

    virtual void addSuffix(uint32_t suffix, uint8_t suffixLength) override {
        addSuffix(T(suffix), suffixLength);
    }
};

}