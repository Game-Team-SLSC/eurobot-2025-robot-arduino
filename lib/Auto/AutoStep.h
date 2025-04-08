#pragma once

#include <Timing.h>

enum AutoStepType {
    TIMED_STEP,
    TRIGGERED_STEP
};

struct AutoStep {
    void(*callback)(void);
    bool wasCalled = false;
    bool isFinished = false;

    const virtual byte getType() const = 0;

    virtual ~AutoStep() = default;
    AutoStep() = default;
    AutoStep(void(*callback)(void)) :
        callback(callback)
    {}
};

struct TriggeredAutoStep: public AutoStep {
    bool (*checker)();

    const byte getType() const override {
        return AutoStepType::TRIGGERED_STEP;
    }

    ~TriggeredAutoStep() = default;
    TriggeredAutoStep() = default;
    TriggeredAutoStep(void(*callback)(void), bool (*checker)()) :
        AutoStep(callback),
        checker(checker)
    {}
};

struct TimedAutoStep: public AutoStep
{
    unsigned long duration;
    unsigned long startTime;

    const byte getType() const override {
        return AutoStepType::TIMED_STEP;
    }

    ~TimedAutoStep() = default;
    TimedAutoStep() = default;
    TimedAutoStep(void(*callback)(void), unsigned long duration) :
        AutoStep(callback),
        duration(duration),
        startTime(0)
    {}
};
