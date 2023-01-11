#pragma once

// abstract match field class

#include <memory>
#include <ostream>

#include "match_type.hpp"

namespace flowbench {

class MatchField {
public:
    virtual MatchType getMatchType() const = 0;
    virtual bool operator== (const MatchField& other) const = 0;
    virtual bool operator!= (const MatchField& other) const = 0;
    virtual bool overlap(const MatchField& other) const = 0;
    virtual bool cover(const MatchField& other) const = 0;
    virtual bool difference(const MatchField& other, std::vector<std::unique_ptr<MatchField>>& out) const = 0;
    virtual bool isWildcard() const = 0;
    virtual std::unique_ptr<MatchField> clone() const = 0;
    virtual void convertFrom(const MatchField& other) {} // convert from other field, for field instantiation (LPM/RM)
    virtual void randomize() {}                          // set a random value, for field instantiation (EM)
    virtual void setParent(const MatchField& parent) {}  // set parent field, for rule instantiation
    virtual void addSuffix(uint32_t suffix, uint8_t suffixLength) {} // add suffix to field, for rule split
    virtual std::unique_ptr<Integer> hit() const { return nullptr; } // return a random value that hits the field

    virtual uint8_t getAvailableWidth(uint8_t width) const {
        return 0;
    }

public:
    virtual void print(std::ostream& os) const {
        os << "Unknown-field";
    }

    virtual void load(std::istream& is) {}
};

}