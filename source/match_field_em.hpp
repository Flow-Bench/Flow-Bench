#pragma once

// EM match field

#include "match_field_sized.hpp"
#include "random.hpp"

namespace flowbench {

template <class T> // where T : Integer
class EmField : public SizedField<T> {
private:
    T value;
    bool wildcard;

public:
    EmField() : value(getZeroOf<T>()), wildcard(true) {}
    EmField(T value) : value(value), wildcard(false) {}
    EmField(const EmField& other) = default;
    EmField(EmField&& other) = default;
    EmField& operator=(const EmField& other) = default;
    EmField& operator=(EmField&& other) = default;
    std::unique_ptr<MatchField> clone() const override {
        return std::make_unique<EmField>(*this);
    }

protected:
    T getMin() const override {
        if (wildcard) {
            return getZeroOf<T>();
        } else {
            return value;
        }
    }

    T getMax() const override {
        if (wildcard) {
            return getMaxOf<T>();
        } else {
            return value;
        }
    }

public:
    MatchType getMatchType() const override {
        return MatchType::EM;
    }

    bool isWildcard() const override {
        return wildcard;
    }

    const T& getValue() const {
        return value;
    }

public:
    // set a random value
    void randomize() override {
        wildcard = false;
        value = Random::getInstance().nextAs<T>();
    }

    void setParent(const MatchField& parent) override;
    std::unique_ptr<Integer> hit() const override {
        if (wildcard) {
            return std::make_unique<T>(Random::getInstance().nextAs<T>());
        } else {
            return std::make_unique<T>(value);
        }
    }

public:
    // EM fields do not contribute to the QuadDag relationship
    virtual bool difference(const MatchField& other, std::vector<std::unique_ptr<MatchField>>& out) const override {
        out.push_back(clone());
        return true;
    }

public:
    // for EM fields
    // 1. wildcard
    //    1.1. FlowBench's default style: *
    //    1.2. ClassBench style:          0x00/0x00
    // 2. exact match
    //    2.1. FlowBench's default style: 0x11
    //    2.2. ClassBench style:          0x11/0xff
    // will be implemented in rule_output.hpp
    void print(std::ostream& os) const override;

    // will be implemented in rule_input.hpp
    void load(std::istream& is) override;

};

template <class T>
void EmField<T>::setParent(const MatchField& parent) {
    const auto& parentEm = dynamic_cast<const EmField<T>&>(parent);
    if (!parentEm.isWildcard()) {
        wildcard = false;
        value = parentEm.getValue();
    }
}

}