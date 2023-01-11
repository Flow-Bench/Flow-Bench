#pragma once

// we use normal distribution as the weights to select a QuadDag

#include <cmath>

namespace flowbench {

class NormalDistribution {
private:
    constexpr static double PI = 3.14159265358979323846;
    
    double mean;
    double variance;

public:
    NormalDistribution(double mean, double variance) : mean(mean), variance(variance) {}

    double getMean() const {
        return mean;
    }

    double getVariance() const {
        return variance;
    }

    double getProbability(double x) const {
        return 1.0 / std::sqrt(2.0 * PI * variance) * std::exp(-std::pow(x - mean, 2) / (2.0 * variance));
    }

};

}