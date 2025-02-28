#include <Action.h>
#include <Timing.h>
#include <Movement.h>
#include <Logger.h>

Action::Action(MovementCall (*movementCalls), byte moveCount):
isExecuting(false),
movementCalls(movementCalls),
moveCount(moveCount),
currentMovementIndex(0)
{}

void Action::execute() {
    currentMovementIndex = 0;
    isExecuting = true;
}

void Action::update() {
    if (!isExecuting) return;

    MovementCall movementCall = (*movementCalls)[currentMovementIndex];
    MovementStatus status = movementCall.movement->getStatus();
    bool target = movementCall.movement->getTarget();

    if (target != movementCall.desiredState || status == MovementStatus::IDLE) {
        execMovement();
        info("Executing move %d", currentMovementIndex);
    }

    info("Action update - update movement ");

    movementCall.movement->update();

    while (mustExecNextMovement()) {
        delay(1000);
        info("Action update - exec next movement");
        ++currentMovementIndex;
        execMovement();
    }
}

bool Action::mustExecNextMovement() {
    // verify if current is last
    bool isLast = currentMovementIndex + 1 >= moveCount;

    // verify if current is done
    MovementCall movementCall = (*movementCalls)[currentMovementIndex];
    MovementStatus status = movementCall.movement->getStatus();
    
    warn("status of mov : ");

    if (isLast && status == MovementStatus::SET) {
        isExecuting = false;
        return false;
    };

    if (status != MovementStatus::SET) return false;

    return true;
}

void Action::execMovement() {
    MovementCall movementCall = (*movementCalls)[currentMovementIndex];

    warn("here");
    delay(1000);

    movementCall.movement->exec(movementCall.desiredState);
}