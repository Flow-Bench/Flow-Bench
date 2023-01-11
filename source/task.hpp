#pragma once

#include "singleton.hpp"
#include "task_type.hpp"
#include "parameter_calculator.hpp"

namespace flowbench {

class Task : public Singleton<Task> {
private:
    TaskType type = TaskType::Unknown;
    uint32_t value = 0;
    double relativeValue;
    bool isRelative = false;

public:
    Task() = default;
    
    TaskType getType() const {
        return type;
    }

    uint32_t getValue() const {
        return value;
    }

    void setType(TaskType type) {
        this->type = type;
    }

    void setValue(uint32_t value) {
        this->value = value;
    }

    void setRelativeValue(double relativeValue) {
        this->relativeValue = relativeValue;
        this->isRelative = true;
    }

    void specifyRelativeValue(uint32_t n) {
        uint32_t maxParameter = ParameterCalculator::getInstance().at(n);
        if (isRelative) {
            value = (uint32_t) (relativeValue * maxParameter);
        }
    }

};

}