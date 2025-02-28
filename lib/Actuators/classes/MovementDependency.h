#pragma once

#include "../enums/MovementName.h"
#include "../enums/ActuatorStatus.h"

struct MovementDependency {
    public:
    
    MovementName movement;
    ActuatorStatus desiredStatus;

    MovementDependency() = default;
    MovementDependency(MovementName movement, ActuatorStatus desiredStatus):
    movement(movement),
    desiredStatus(desiredStatus)
    {}
};