#pragma once

// read the parameters specified by the user
// (for trace generator)
// the parameters we support can be found in the document

#include <iostream>
#include <cstring>
#include <vector>

#include "singleton.hpp"
#include "protocol.hpp"
#include "rule_format.hpp"
#include "pareto_distribution.hpp"

namespace flowbench {

class TraceConfiguration : public Singleton<TraceConfiguration> {
private:
    // the number of traces in the flow table (-n)
    uint32_t traceCount = 0;

    // the number of rules in the flow table (cannot be specified directly)
    uint32_t ruleCount = 0;

    // the density of the traces (-d)
    double traceDensity = 0.0;

    // the input file path (-i)
    std::string inputFilePath;

    // the output file path (-o)
    std::string outputFilePath;

    // the user-defined protocol is set in RuleTypeUd (-f, -fw, -ft)

    // the spatial locality of the traces is set in ParetoDistribution (-rd, -fd)

    // the random seed (-s)
    uint32_t randomSeed = 5489;

    // user-defined output style (--flowbench/--classbench)
    RuleOutputStyle outputStyle = RuleOutputStyle::FlowBench;

    // enable fast mode (--fast)
    bool fastMode = false;

    // whether use pre-defined protocol is set in RuleTypeUd (-p)

public:
    TraceConfiguration() = default;
    TraceConfiguration(int argc, char* argv[]); // read the parameters specified by the user

    // print the configuration
    void print(std::ostream& os) const;

private:
    void applyDefaultConfiguration();

public:
    uint32_t getTraceCount() const {
        return traceCount;
    }

    void setRuleCount(uint32_t ruleCount) {
        this->ruleCount = ruleCount;
        if (traceDensity > 0.0 && traceCount == 0) {
            traceCount = static_cast<uint32_t>(ruleCount * traceDensity);
        }
    }

    uint32_t getRuleCount() const {
        return ruleCount;
    }

    const char* getInputFilePath() const {
        return inputFilePath.c_str();
    }

    const char* getOutputFilePath() const {
        return outputFilePath.c_str();
    }

    auto getOutputStyle() const {
        return outputStyle;
    }

    bool enableFastMode() const {
        return fastMode;
    }
};

TraceConfiguration::TraceConfiguration(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) {
            traceCount = std::stoul(argv[++i]);
        } else if (strcmp(argv[i], "-d") == 0) {
            traceDensity = std::stod(argv[++i]);
        } else if (strcmp(argv[i], "-i") == 0) {
            inputFilePath = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0) {
            outputFilePath = argv[++i];
        } else if (strcmp(argv[i], "-f") == 0) {
            RuleTypeUD::getInstance().setFieldCount(std::stoul(argv[++i]));
        } else if (strcmp(argv[i], "-fw") == 0) {
            for (uint8_t j = 0; j < RuleTypeUD::getInstance().getFieldCount(); j++) {
                RuleTypeUD::getInstance().setFieldWidth(j, std::stoul(argv[++i]));
            }
        } else if (strcmp(argv[i], "-ft") == 0) {
            for (uint8_t j = 0; j < RuleTypeUD::getInstance().getFieldCount(); j++) {
                RuleTypeUD::getInstance().setMatchType(j, getEnumValue<MatchType>(argv[++i]));
            }
        } else if (strcmp(argv[i], "-rd") == 0) {
            ParetoDistribution::ruleDistribution.setAlpha(std::stod(argv[++i]));
            ParetoDistribution::ruleDistribution.setBeta(std::stod(argv[++i]));
        } else if (strcmp(argv[i], "-fd") == 0) {
            ParetoDistribution::flowDistribution.setAlpha(std::stod(argv[++i]));
            ParetoDistribution::flowDistribution.setBeta(std::stod(argv[++i]));
        } else if (strcmp(argv[i], "-s") == 0) {
            randomSeed = std::stoul(argv[++i]);
        } else if (strcmp(argv[i], "--flowbench") == 0) {
            outputStyle = RuleOutputStyle::FlowBench;
        } else if (strcmp(argv[i], "--classbench") == 0) {
            outputStyle = RuleOutputStyle::ClassBench;
        } else if (strcmp(argv[i], "--fast") == 0) {
            fastMode = true;
        } else if (strcmp(argv[i], "-p") == 0) {
            RuleTypeUD::getInstance().setProtocol(getEnumValue<Protocol>(argv[++i]));
        } else {
            std::cerr << "Unknown parameter: " << argv[i] << std::endl;
            exit(1);
        }
    }
    applyDefaultConfiguration();
}

void TraceConfiguration::applyDefaultConfiguration() {
    if (traceCount == 0 && traceDensity == 0.0) {
        traceCount = 1000;
    }
    if (ParetoDistribution::ruleDistribution.getAlpha() == 0.0) {
        fastMode = true;
    }
    if (inputFilePath.empty()) {
        inputFilePath = "input.txt";
    }
    if (outputFilePath.empty()) {
        outputFilePath = inputFilePath + "_trace";
    }
    if (RuleTypeUD::getInstance().getFieldCount() == 0) {
        RuleTypeUD::getInstance().setProtocol(Protocol::IPv4);
    }
    Random::setInstance(randomSeed);
    RuleFormat::outputFormat.setStyle(outputStyle);
}

void TraceConfiguration::print(std::ostream& os) const {
    os << "Trace Count: " << traceCount << std::endl;
    os << "Trace Density: " << traceDensity << std::endl;
    os << "Input File Path: " << inputFilePath << std::endl;
    os << "Output File Path: " << outputFilePath << std::endl;
    os << "Field Count: " << (uint32_t) RuleTypeUD::getInstance().getFieldCount() << std::endl;
    os << "Field Widths: ";
    for (uint8_t i = 0; i < RuleTypeUD::getInstance().getFieldCount(); i++) {
        os << (uint32_t) RuleTypeUD::getInstance().getFieldWidth(i) << " ";
    }
    os << std::endl;
    os << "Match Types: ";
    for (uint8_t i = 0; i < RuleTypeUD::getInstance().getFieldCount(); i++) {
        os << getEnumName(RuleTypeUD::getInstance().getMatchType(i)) << " ";
    }
    os << std::endl;
    os << "Rule Distribution: Pareto(" << ParetoDistribution::ruleDistribution.getAlpha() << ", "
       << ParetoDistribution::ruleDistribution.getBeta() << ")" << std::endl;
    os << "Flow Distribution: Pareto(" << ParetoDistribution::flowDistribution.getAlpha() << ", "
       << ParetoDistribution::flowDistribution.getBeta() << ")" << std::endl;
    os << "Random Seed: " << randomSeed << std::endl;
    os << "Output Style: " << getEnumName(outputStyle) << std::endl;
}

}