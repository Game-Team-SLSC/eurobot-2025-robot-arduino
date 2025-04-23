#pragma once

#include <Arduino.h>
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
    unsigned long startTime;
    
    const virtual byte getType() const = 0;

    Movement(void(*callback)(), MovementDependency* dependencies, byte dependencyCount):
    callback(callback),
    dependencies(dependencies),
    dependencyCount(dependencyCount),
    startTime(0)
    {}
};

struct TimedMovement: public Movement {
    unsigned int duration;
    
    const byte getType() const override {
        return MovementType::TIMED;
    }
    
    TimedMovement(void(*callback)(), unsigned int duration, MovementDependency* dependencies, byte dependencyCount):
    Movement(callback, dependencies, dependencyCount),
    duration(duration)
    {}
};

struct TriggeredMovement: public Movement {
    bool(*checker)();
    unsigned int timeout;
    
    const byte getType() const override {
        return MovementType::TRIGGERED;
    }

    TriggeredMovement(void(*callback)(), bool(*checker)(), unsigned int timeout, MovementDependency* dependencies, byte dependencyCount):
    Movement(callback, dependencies, dependencyCount),
    checker(checker),
    timeout(timeout)
    {}
};