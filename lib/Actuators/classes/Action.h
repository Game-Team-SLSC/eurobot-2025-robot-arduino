#pragma once

#include <Arduino.h>
#include "MovementDependency.h"

struct Action {
    MovementDependency* steps;
    byte stepCount;
    
    Action(MovementDependency* steps, byte count): 
        steps(steps), 
        stepCount(count) 
    {}
};