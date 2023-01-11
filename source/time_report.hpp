#pragma once

// report the time of a function
// return the time in seconds

#include <functional>
#include <chrono>

namespace flowbench {

double reportTime(std::function<void()> func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    return duration.count() / 1e6;
}

}