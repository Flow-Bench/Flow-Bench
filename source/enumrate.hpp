#pragma once

// a enumarate template
// usage:
// when you want to use enumarate, you should define a enum class
// and define a function to get the names of the enum class
// override the getNames function to return const std::unordered_map<Enum, const char*>*

#include <unordered_map>
#include <cstring>

namespace flowbench {

template <class T>
const auto getNames() {
    return nullptr;
}

template <class T>
const auto getDefaultValue() {
    return T::Unknown;
}

template <class T>
const auto getDefaultName() {
    return "Unknown";
}

template <class T>
const auto getEnumName(T value) {
    auto names = getNames<T>();
    if (names == nullptr) {
        return getDefaultName<T>();
    }
    auto it = names->find(value);
    if (it == names->end()) {
        return getDefaultName<T>();
    }
    return it->second;
}

template <class T>
const auto getEnumValue(const char* name) {
    auto names = getNames<T>();
    if (names == nullptr) {
        return getDefaultValue<T>();
    }
    for (auto it = names->begin(); it != names->end(); it++) {
        if (strcmp(it->second, name) == 0) {
            return it->first;
        }
    }
    return getDefaultValue<T>();
}

}