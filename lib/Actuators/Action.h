#pragma once

#include <MovementCall.h>
#include <MovementStatus.h>
#include <GlobalSettings.h>
#include <Arduino.h>

#define MAX_MOVEMENTS 

class Action {
    public:

    Action(MovementCall movementCalls[], byte moveCount);
    ~Action() = default;
    
    void execute();
    void update();

    private:

    bool mustExecNextMovement() const;
    void execMovement();

    const MovementCall* movementCalls;
    const byte moveCount;
    byte currentMovementIndex;
};