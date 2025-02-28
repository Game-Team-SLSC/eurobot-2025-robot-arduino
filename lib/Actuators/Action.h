#pragma once

#include <MovementCall.h>
#include <MovementStatus.h>
#include <GlobalSettings.h>
#include <Arduino.h>
#include <DynamicStateController.h>

#define MAX_MOVEMENTS 15

class Action {
    public:

    Action(MovementCall *movementCalls, byte moveCount);
    ~Action() = default;
    
    void execute();
    void update();
    bool isExecuting;

    private:

    bool mustExecNextMovement();
    void execMovement();

    MovementCall (*movementCalls)[];
    const byte moveCount;
    byte currentMovementIndex;
};