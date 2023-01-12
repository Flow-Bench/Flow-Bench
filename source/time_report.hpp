#pragma once

// report the time of a function
// return the time in seconds

#include <functional>
#include <chrono>
#include <map>

namespace flowbench {

double reportTime(std::function<void()> func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    return duration.count() / 1e6;
}

// for debug
// record the time of a function
class TimeRecorder : public std::map<const char*, double>, public Singleton<TimeRecorder> {
public:
    TimeRecorder() = default;

    void record(const char* name, std::function<void()> func) {
        if (find(name) == end()) {
            (*this)[name] = reportTime(func);
        } else {
            (*this)[name] += reportTime(func);
        }
    }

    void report(std::ostream& os) const {
        for (const auto& pair : *this) {
            os << pair.first << ": " << pair.second << "s" << std::endl;
        }
    }

};

}