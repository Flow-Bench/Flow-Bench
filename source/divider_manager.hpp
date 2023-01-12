#pragma once

#include <map>

#include "singleton.hpp"
#include "divider.hpp"

namespace flowbench {

class DividerManager : public Singleton<DividerManager> {
private:
    std::map<uint32_t, Divider> dividers;

    uint32_t cache;
    Divider* last = nullptr;

public:
    DividerManager() {
        dividers.emplace(0, Divider(0));
        cache = 0;
        last = &dividers[0];
    }
    const Divider& getDivider(uint32_t n);

};

const Divider& DividerManager::getDivider(uint32_t n) {
    if (n == cache) {
        return *last;
    }
    auto it = dividers.find(n);
    if (it != dividers.end()) {
        cache = n;
        last = &it->second;
        return *last;
    }
    auto result = dividers.emplace(n, Divider(n));
    cache = n;
    last = &result.first->second;
    return *last;
}



}