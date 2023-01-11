#pragma once

// we use Pareto distribution to control the spatial locality of the traces

#include <cmath>

#include "random.hpp"

namespace flowbench {

class ParetoDistribution {
private:
    double alpha;
    double beta;

public:
    ParetoDistribution(double alpha, double beta) : alpha(alpha), beta(beta) {}

    double getAlpha() const {
        return alpha;
    }

    double getBeta() const {
        return beta;
    }

    void setAlpha(double alpha) {
        this->alpha = alpha;
    }

    void setBeta(double beta) {
        this->beta = beta;
    }

    uint32_t copyCount() const {
        if (beta == 0.0) {
            return 1;
        }
        double p = Random::getInstance().nextDouble(0.0, 1.0 - 1e-9);
        double result = beta / std::pow(1.0 - p, 1.0 / alpha);
        return std::ceil(result);
    }

    static ParetoDistribution ruleDistribution;
    static ParetoDistribution flowDistribution;

};

ParetoDistribution ParetoDistribution::ruleDistribution(0.0, 0.0);
ParetoDistribution ParetoDistribution::flowDistribution(1.0, 1.0);

}