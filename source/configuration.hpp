#pragma once

// read the parameters specified by the user
// the parameters we support can be found in the document

#include <iostream>
#include <cstring>
#include <vector>

#include "singleton.hpp"
#include "protocol.hpp"
#include "rule_output_style.hpp"
#include "task.hpp"

namespace flowbench {

class Configuration : public Singleton<Configuration> {
private:
    // the number of rules in the flow table (-n)
    uint32_t ruleCount = 0;

    // the output file path (-o)
    std::string outputFilePath;

    // the user-defined protocol is set in RuleTypeUd (-f, -fw, -ft)

    // the weight of the fields (-fwt)
    std::vector<double> fieldWeights;

    // D or E requirement is set in Task (-d, -e)

    // the random seed (-s)
    uint32_t randomSeed = 5489;

    // user-defined output style (--flowbench/--classbench)
    RuleOutputStyle outputStyle = RuleOutputStyle::FlowBench;

    // whether enable arbitrary range feature (-ar)
    bool enableArbitraryRange = false;

    // whether enable dense mode
    bool enableDenseMode = false;

    // whether use pre-defined protocol is set in RuleTypeUd (-p)

public:
    Configuration() = default;
    Configuration(int argc, char* argv[]); // read the parameters specified by the user

    // print the configuration
    void print(std::ostream& os) const;

private:
    void applyDefaultConfiguration();

public:
    uint32_t getRuleCount() const {
        return ruleCount;
    }

    const char* getOutputFilePath() const {
        return outputFilePath.c_str();
    }

    double getFieldWeight(uint8_t fieldIndex) const {
        if (fieldIndex >= fieldWeights.size()) {
            return 1.0;
        } else {
            return fieldWeights[fieldIndex];
        }
    }

    auto getOutputStyle() const {
        return outputStyle;
    }

    bool isEnableArbitraryRange() const {
        return enableArbitraryRange;
    }

    const char* getQuadDagFilePath() const {
        if (enableDenseMode) {
            return DENSE_PROFILE_PATH;
        } else {
            return NORMAL_PROFILE_PATH;
        }
    }

};

Configuration::Configuration(int argc, char* argv[]) {
    fieldWeights.clear();
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) {
            ruleCount = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-o") == 0) {
            outputFilePath = argv[++i];
        } else if (strcmp(argv[i], "-f") == 0) {
            RuleTypeUD::getInstance().setFieldCount(atoi(argv[++i]));
        } else if (strcmp(argv[i], "-fw") == 0 || strcmp(argv[i], "--field-width") == 0) {
            for (uint8_t j = 0; j < RuleTypeUD::getInstance().getFieldCount(); j++) {
                RuleTypeUD::getInstance().setFieldWidth(j, atoi(argv[++i]));
            }
        } else if (strcmp(argv[i], "-ft") == 0 || strcmp(argv[i], "--field-type") == 0) {
            for (uint8_t j = 0; j < RuleTypeUD::getInstance().getFieldCount(); j++) {
                RuleTypeUD::getInstance().setMatchType(j, getEnumValue<MatchType>(argv[++i]));
            }
        } else if (strcmp(argv[i], "-fwt") == 0 || strcmp(argv[i], "--field-weight") == 0) {
            fieldWeights.resize(RuleTypeUD::getInstance().getFieldCount());
            for (uint8_t j = 0; j < RuleTypeUD::getInstance().getFieldCount(); j++) {
                fieldWeights[j] = atof(argv[++i]);
            }
        } else if (strcmp(argv[i], "-D") == 0) {
            Task::getInstance().setType(TaskType::DependencyLength);
            Task::getInstance().setValue(atoi(argv[++i]));
        } else if (strcmp(argv[i], "-E") == 0) {
            Task::getInstance().setType(TaskType::EdgeCount);
            Task::getInstance().setValue(atoi(argv[++i]));
        } else if (strcmp(argv[i], "-d") == 0) {
            Task::getInstance().setType(TaskType::DependencyLength);
            Task::getInstance().setRelativeValue(atof(argv[++i]));
        } else if (strcmp(argv[i], "-e") == 0) {
            Task::getInstance().setType(TaskType::EdgeCount);
            Task::getInstance().setRelativeValue(atof(argv[++i]));
        } else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--random-seed") == 0) {
            randomSeed = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--flowbench") == 0) {
            outputStyle = RuleOutputStyle::FlowBench;
        } else if (strcmp(argv[i], "--classbench") == 0) {
            outputStyle = RuleOutputStyle::ClassBench;
        } else if (strcmp(argv[i], "-ar") == 0 || strcmp(argv[i], "--arbitrary-range") == 0) {
            enableArbitraryRange = true;
        } else if (strcmp(argv[i], "--dense") == 0) {
            enableDenseMode = true;
        } else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--protocol") == 0) {
            RuleTypeUD::getInstance().setProtocol(getEnumValue<Protocol>(argv[++i]));
        } else {
            std::cout << "Unknown parameter: " << argv[i] << std::endl;
            exit(1);
        }
    }
    applyDefaultConfiguration();
}

void Configuration::applyDefaultConfiguration() {
    if (ruleCount == 0) {
        ruleCount = DEFAULT_RULE_CNT;
    }
    if (outputFilePath.empty()) {
        outputFilePath = std::to_string(ruleCount) + ".txt";
    }
    if (RuleTypeUD::getInstance().getFieldCount() == 0) {
        RuleTypeUD::getInstance().setProtocol(Protocol::IPv4);
    }
    if (fieldWeights.empty()) {
        fieldWeights.resize(RuleTypeUD::getInstance().getFieldCount(), 1.0);
        for (uint8_t i = 0; i < RuleTypeUD::getInstance().getFieldCount(); i++) {
            if (RuleTypeUD::getInstance().getMatchType(i) != MatchType::EM) {
                fieldWeights[i] = RuleTypeUD::getInstance().getFieldWidth(i);
            }
        }
    }
    Random::setInstance(randomSeed);
    if (Task::getInstance().getType() == TaskType::Unknown) {
        Task::getInstance().setType(TaskType::DependencyLength);
        Task::getInstance().setRelativeValue(Random::getInstance().nextDouble(0.0, 1.0));
    }
    ParameterCalculator::setInstance(ruleCount);
    Task::getInstance().specifyRelativeValue(ruleCount);
}

void Configuration::print(std::ostream& os) const {
    os << "ruleCount: " << ruleCount << std::endl;
    os << "outputFilePath: " << outputFilePath << std::endl;
    os << "fieldCount: " << (uint32_t) RuleTypeUD::getInstance().getFieldCount() << std::endl;
    os << "fieldWidths: ";
    for (uint8_t i = 0; i < RuleTypeUD::getInstance().getFieldCount(); i++) {
        os << (uint32_t) RuleTypeUD::getInstance().getFieldWidth(i) << " ";
    }
    os << std::endl;
    os << "fieldTypes: ";
    for (uint8_t i = 0; i < RuleTypeUD::getInstance().getFieldCount(); i++) {
        os << getEnumName(RuleTypeUD::getInstance().getMatchType(i)) << " ";
    }
    os << std::endl;
    os << "fieldWeights: ";
    for (uint8_t i = 0; i < RuleTypeUD::getInstance().getFieldCount(); i++) {
        os << fieldWeights[i] << " ";
    }
    os << std::endl;
    os << "taskType: " << getEnumName(Task::getInstance().getType()) << std::endl;
    os << "taskValue: " << Task::getInstance().getValue() << std::endl;
    os << "randomSeed: " << randomSeed << std::endl;
    os << "outputStyle: " << getEnumName(outputStyle) << std::endl;
    os << "enableArbitraryRange: " << enableArbitraryRange << std::endl;
    os << "enableDenseMode: " << enableDenseMode << std::endl;
}

}