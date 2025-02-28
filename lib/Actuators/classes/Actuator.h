#pragma once

#include "../enums/MovementName.h"
#include "../enums/ActuatorStatus.h"

struct Actuator {
    const MovementName* moves;
    const byte moveCount;
    ActuatorStatus status;
    MovementName targetMovement;

    Actuator(MovementName moves[], byte moveCount) :
    moves(moves),
    moveCount(moveCount),
    status(ActuatorStatus::IDLE),
    targetMovement(moves[0])
    {};
};