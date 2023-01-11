#pragma once

#include <queue>

#include "exception.hpp"
#include "random.hpp"
#include "match_field_sized.hpp"

namespace flowbench {

template <class T> // where T : Integer
class LpmField : public SizedField<T> {
private:
    T prefix;
    uint8_t prefixLength;

public:
    LpmField() : prefix(getZeroOf<T>()), prefixLength(0) {}
    LpmField(T prefix, uint8_t prefixLength) : prefix(prefix), prefixLength(prefixLength) {}
    LpmField(const LpmField& other) = default;
    LpmField(LpmField&& other) = default;
    LpmField& operator=(const LpmField& other) = default;
    LpmField& operator=(LpmField&& other) = default;
    LpmField(const std::string& str); // read from pre-computed candidate file
    std::unique_ptr<MatchField> clone() const override {
        return std::make_unique<LpmField>(*this);
    }

    // convert Candidate rule to User-defined rule
    void convertFrom(const MatchField& other) override {
        const LpmField<Int32>& otherLpm = dynamic_cast<const LpmField<Int32>&>(other);
        prefix = otherLpm.prefix;
        prefixLength = otherLpm.prefixLength;
    }

    void setParent(const MatchField& parent) override;
    std::unique_ptr<Integer> hit() const override {
        T suffix = Random::getInstance().nextAs<T>() >> prefixLength;
        return std::make_unique<T>(prefix | suffix);
    }

private:
    T getMask() const {
        if (prefixLength == 0) {
            return getZeroOf<T>();
        } else {
            return getMaxOf<T>() << (getBitCount<T>() - prefixLength);
        }
    }

protected:
    T getMin() const override {
        return prefix;
    }

    T getMax() const override {
        return prefix | (getMaxOf<T>() >> prefixLength);
    }

public:
    MatchType getMatchType() const override {
        return MatchType::LPM;
    }

    bool isWildcard() const override {
        return prefixLength == 0;
    }

    const T& getPrefix() const {
        return prefix;
    }

    uint8_t getPrefixLength() const {
        return prefixLength;
    }

    uint8_t getAvailableWidth(uint8_t width) const override {
        return width - prefixLength;
    }

public:
    std::unique_ptr<LpmField<T>> mergeWith(const LpmField& other) const;
    LpmField& operator^=(const T& xorMask);
    void addSuffix(const T& suffix, uint8_t suffixLength) override;

public:
    virtual bool difference(const MatchField& other, std::vector<std::unique_ptr<MatchField>>& out) const override;

public:
    // output format for LPM fields
    // 1. wildcard
    //    1.1. FlowBench's default style: *
    //    1.2. ClassBench style:          0.0.0.0/0
    // 2. LPM
    //    2.1. FlowBench's default style: 1000
    //    2.2. ClassBench style:          128.0.0.0/4
    // will be implemented in rule_output.hpp
    void print(std::ostream& os) const override;

    // will be implemented in rule_input.hpp
    void load(std::istream& is) override;
};

template <class T>
LpmField<T>::LpmField(const std::string& str) {
    auto len = str.size();
    prefix = getZeroOf<T>();
    prefixLength = 0;
    if (len > 0 && str[0] != '*') {
        prefixLength = len;
        auto bitMask = getHighestBitOf<T>();
        for (auto i = 0; i < len; i++, bitMask >>= 1) {
            if (str[i] == '1') {
                prefix |= bitMask;
            }
        }
    }
}

template <class T>
std::unique_ptr<LpmField<T>> LpmField<T>::mergeWith(const LpmField& other) const {
    if (prefixLength == 0 || prefixLength != other.prefixLength) {
        return nullptr;
    } else {
        auto mask = getMask() << 1;
        if ((prefix & mask) != (other.prefix & mask)) {
            return nullptr;
        } else {
            return std::make_unique<LpmField<T>>(prefix & mask, prefixLength - 1);
        }
    }
}

template <class T>
LpmField<T>& LpmField<T>::operator^=(const T& xorMask) {
    prefix ^= xorMask;
    prefix &= getMask();
    return *this;
}

template <class T>
void LpmField<T>::addSuffix(const T& suffix, uint8_t suffixLength) {
    if ((prefixLength += suffixLength) > getBitCount<T>()) {
        throw BitWidthError();
    } else {
        prefix |= suffix << (getBitCount<T>() - prefixLength);
    }
}

template <class T>
void LpmField<T>::setParent(const MatchField& parent) {
    const auto& parentLpm = dynamic_cast<const LpmField<T>&>(parent);
    if (!parentLpm.isWildcard()) {
        prefix = parentLpm.getPrefix() | (prefix >> parentLpm.getPrefixLength());
        prefixLength += parentLpm.getPrefixLength();
    }
}

template <class T>
bool LpmField<T>::difference(const MatchField& other, std::vector<std::unique_ptr<MatchField>>& out) const  {
    if (!cover(other)) {
        return false;
    }
    std::unique_ptr<LpmField<T>> current = clone();
    while (current != other) {
        auto left = current.clone();
        auto right = current.clone();
        left->addSuffix(0, 1);
        right->addSuffix(1, 1);
        if (left->cover(other)) {
            out.push_back(std::move(right));
            current = std::move(left);
        } else {
            out.push_back(std::move(left));
            current = std::move(right);
        }
    }
    return true;
}

}