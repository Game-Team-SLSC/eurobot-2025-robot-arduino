#pragma once

#include <Arduino.h>
#include <Timing.h>
#include "../enums/ActuatorName.h"
#include "MovementDependency.h"

enum MovementType {
    TIMED,
    TRIGGERED
};

struct Movement {
    void(*callback)();
    MovementDependency* dependencies;
    byte dependencyCount;
    
    const virtual byte getType() const = 0;

    Movement(void(*callback)(), MovementDependency* dependencies, byte dependencyCount):
    callback(callback),
    dependencies(dependencies),
    dependencyCount(dependencyCount)
    {}
};

struct TimedMovement: public Movement {
    unsigned int duration;
    InTimer* timer;
    
    const byte getType() const override {
        return MovementType::TIMED;
    }
    
    TimedMovement(void(*callback)(), unsigned int duration, MovementDependency* dependencies, byte dependencyCount):
    Movement(callback, dependencies, dependencyCount),
    duration(duration),
    timer(nullptr)
    {}
};

struct TriggeredMovement: public Movement {
    bool(*checker)(void*);
    unsigned int timeout;
    InTimer* timeoutTimer;
    WhenTimer* checkerTask;
    
    const byte getType() const override {
        return MovementType::TRIGGERED;
    }

    TriggeredMovement(void(*callback)(), bool(*checker)(void*), unsigned int timeout, MovementDependency* dependencies, byte dependencyCount):
    Movement(callback, dependencies, dependencyCount),
    checker(checker),
    timeout(timeout),
    timeoutTimer(nullptr),
    checkerTask(nullptr)
    {}
};