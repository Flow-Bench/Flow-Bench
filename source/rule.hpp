#pragma once

// a class for rules

#include <functional>

#include "rule_type_candidate.hpp"
#include "rule_type_ud.hpp"
#include "edge_type.hpp"

namespace flowbench {

template <class T> // where T : RuleType
class Rule {
private:
    std::vector<std::unique_ptr<MatchField>> fields;

public:
    Rule();
    Rule(const Rule& other);

public:
    MatchField& getField(uint8_t fieldIndex) const {
        return *fields[fieldIndex];
    }

    template <typename U> // where U : MatchField
    U& getFieldAs(uint8_t fieldIndex) const {
        return dynamic_cast<U&>(*fields[fieldIndex]);
    }

    void setField(uint8_t fieldIndex, std::unique_ptr<MatchField> field) {
        fields[fieldIndex] = std::move(field);
    }

    RuleType& getRuleType() const {
        return T::getInstance();
    }

    uint8_t getFieldCount() const {
        return getRuleType().getFieldCount();
    }

    // the following functions are used to analyze the relationship of two rules
private:
    bool compareFields(const Rule& other, std::function<bool(const MatchField&, const MatchField&)> test) const;
public:
    bool overlap(const Rule& other) const;
    bool cover(const Rule& other) const;
    bool operator==(const Rule& other) const;
    EdgeType getEdgeTypeTo(const Rule& other) const;

    // the following functions are used to analyze the attributes of a rule
public:
    uint8_t getAvailableWidth(uint8_t fieldIndex) const;
    uint8_t getAvailableWidth() const {
        uint32_t width = 0;
        for (uint8_t i = 0; i < getFieldCount(); i++) {
            width += getAvailableWidth(i);
        }
        if (width > UINT8_MAX) {
            return UINT8_MAX;
        }
        return static_cast<uint8_t>(width);
    }

    // Candidate rule -> User-defined rule with a mapping
public:
    Rule(const Rule<RuleTypeCandidate>& other, const std::array<uint8_t, QD_FIELD_CNT>& mapping);


public:
    std::unique_ptr<Rule> clone() const {
        return std::make_unique<Rule>(*this);
    }

    bool isWildcard() const {
        for (uint8_t i = 0; i < getFieldCount(); i++) {
            if (!getField(i).isWildcard()) {
                return false;
            }
        }
        return true;
    }
};

template <class T>
bool Rule<T>::compareFields(const Rule& other, std::function<bool(const MatchField&, const MatchField&)> test) const {
    for (uint8_t i = 0; i < getFieldCount(); i++) {
        if (!test(getField(i), other.getField(i))) {
            return false;
        }
    }
    return true;
}

template <class T>
bool Rule<T>::overlap(const Rule& other) const {
    return compareFields(other, [](const MatchField& a, const MatchField& b) {
        return a.overlap(b);
    });
}

template <class T>
bool Rule<T>::cover(const Rule& other) const {
    return compareFields(other, [](const MatchField& a, const MatchField& b) {
        return a.cover(b);
    });
}

template <class T>
bool Rule<T>::operator==(const Rule& other) const {
    return compareFields(other, [](const MatchField& a, const MatchField& b) {
        return a == b;
    });
}

template <class T>
EdgeType Rule<T>::getEdgeTypeTo(const Rule& other) const {
    if (cover(other)) {
        return EdgeType::Cover;
    } else if (overlap(other)) {
        return EdgeType::Overlap;
    } else {
        return EdgeType::None;
    }
}

template <class T>
Rule<T>::Rule() {
    for (uint8_t i = 0; i < getFieldCount(); i++) {
        fields.push_back(std::move(getRuleType().createField(i)));
    }
}

template <class T>
Rule<T>::Rule(const Rule& other) {
    fields.resize(getFieldCount());
    for (uint8_t i = 0; i < getFieldCount(); i++) {
        fields[i] = std::move(other.getField(i).clone());
    }
}

template <class T>
Rule<T>::Rule(const Rule<RuleTypeCandidate>& other, const std::array<uint8_t, QD_FIELD_CNT>& mapping) {
    fields.resize(getFieldCount());
    std::fill(fields.begin(), fields.end(), nullptr);
    for (uint8_t i = 0; i < other.getFieldCount(); i++) {
        fields[mapping[i]] = std::move(getRuleType().createField(mapping[i]));
        fields[mapping[i]]->convertFrom(other.getField(i));
    }
    for (uint8_t i = 0; i < getFieldCount(); i++) {
        if (fields[i] == nullptr) {
            fields[i] = std::move(getRuleType().createField(i));
        }
    }
}

template <class T>
uint8_t Rule<T>::getAvailableWidth(uint8_t fieldIndex) const {
    return getField(fieldIndex).getAvailableWidth(getRuleType().getFieldWidth(fieldIndex));
}

using CandidateRule = Rule<RuleTypeCandidate>;
using UDRule = Rule<RuleTypeUD>;

}