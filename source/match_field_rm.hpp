#pragma once

#include <cmath>

#include "int32.hpp"
#include "random.hpp"
#include "match_field_sized.hpp"
#include "match_field_lpm.hpp"

namespace flowbench {

template <class T> // where T : Integer
class RmField : public SizedField<T> {
private:
    T start;
    T end;

public:
    RmField() : start(getZeroOf<T>()), end(getMaxOf<T>()) {}
    RmField(T start, T end) : start(start), end(end) {}
    RmField(const RmField& other) = default;
    RmField(RmField&& other) = default;
    RmField& operator=(const RmField& other) = default;
    RmField& operator=(RmField&& other) = default;
    RmField(const SizedField<T>& other) : start(other.getMin()), end(other.getMax()) {}
    std::unique_ptr<MatchField> clone() const override {
        return std::make_unique<RmField>(*this);
    }

    // convert Candidate rule to User-defined rule
    void convertFrom(const MatchField& other) override {
        const LpmField<Int32>& otherLpm = dynamic_cast<const LpmField<Int32>&>(other);
        start = otherLpm.getMin();
        end = otherLpm.getMax();
    }

    void setParent(const MatchField& parent) override;
    void addSuffix(const T& suffix, uint8_t suffixLength) override;
    std::unique_ptr<Integer> hit() const override {
        return std::make_unique<T>(Random::getInstance().nextUInt32(start.getValue(), end.getValue()));
    }

public:
    T getMin() const override {
        return start;
    }

    T getMax() const override {
        return end;
    }

    MatchType getMatchType() const override {
        return MatchType::RM;
    }

public:
    bool difference(const MatchField& other, std::vector<std::unique_ptr<MatchField>>& out) const override;

public:
    // for RM fields
    // both styles : start : end
    // will be implemented in rule_output.hpp
    void print(std::ostream& os) const override;

    // will be implemented in rule_input.hpp
    void load(std::istream& is) override;

    uint8_t getAvailableWidth(uint8_t width) const override {
        return 0;
    }
};

// RM only supports Int32
template <>
uint8_t RmField<Int32>::getAvailableWidth(uint8_t width) const {
    uint64_t start = this->start.getValue();
    uint64_t end = this->end.getValue();
    uint64_t range = (end - start + 1) >> (32 - width);
    return std::floor(std::log2(range));
}

// RM only supports Int32
template <class T>
void RmField<T>::setParent(const MatchField& parent) {
    const auto& parentRm = dynamic_cast<const RmField<Int32>&>(parent);
    if (!parentRm.isWildcard()) {
        uint64_t parentMin = parentRm.getMin().getValue();
        uint64_t parentMax = parentRm.getMax().getValue();
        uint64_t range = parentMax - parentMin + 1;
        uint64_t min = this->start.getValue();
        uint64_t max = this->end.getValue();
        start = T(parentMin + (min * range >> 32));
        end = T(parentMin + (max * range >> 32));
    }
}

// RM only supports Int32
template <class T>
void RmField<T>::addSuffix(const T& suffix, uint8_t suffixLength) {
    uint32_t step = ((start ^ end) >> suffixLength).getValue() + 1;
    start = T(start.getValue() + suffix.getValue() * step);
    end = T(start.getValue() + step - 1);
}

template <class T>
bool RmField<T>::difference(const MatchField& other, std::vector<std::unique_ptr<MatchField>>& out) const {
    const auto& otherRm = static_cast<const RmField<T>&>(other);
    T otherMin = otherRm.getMin();
    T otherMax = otherRm.getMax();
    if (otherMin > start) {
        out.push_back(std::make_unique<RmField<T>>(start, T(otherMin.getValue() - 1)));
    } else if (otherMax < end) {
        out.push_back(std::make_unique<RmField<T>>(T(otherMax.getValue() + 1), end));
    } else {
        return false;
    }
    return true;
}

}