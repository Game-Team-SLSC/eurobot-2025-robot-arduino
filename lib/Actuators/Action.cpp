#include <Action.h>
#include <Timing.h>
#include <Movement.h>

Action::Action(MovementCall* movementCalls, byte moveCount):
movementCalls(movementCalls),
moveCount(moveCount),
currentMovementIndex(0)
{}

void Action::execute() {
    currentMovementIndex = 0;
    update();
}

void Action::update() {
    while (mustExecNextMovement()) {
        ++currentMovementIndex;
        execMovement();
    }
}

bool Action::mustExecNextMovement() const {
    if (currentMovementIndex + 1 >= moveCount) return false;

    const MovementCall& movementCall = movementCalls[currentMovementIndex];
    MovementStatus status = movementCall.movement->getStatus();

    if (status != MovementStatus::SET) return false;

    return true;
}

void Action::execMovement() {
    const MovementCall& movementCall = movementCalls[currentMovementIndex];

    movementCall.movement->exec(movementCall.desiredState);
}