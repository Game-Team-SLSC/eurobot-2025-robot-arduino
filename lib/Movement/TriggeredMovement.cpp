#include <TriggeredMovement.h>
#include <Timing.h>
TriggeredMovement::TriggeredMovement(MovementCallback callback, MovementChecker checker):
Movement(callback),
checker(checker)
{}

void TriggeredMovement::exec(bool active) {
    Movement::exec(active);

    if (!status->hasChanged() && !target->hasChanged()) return;
    
    if (checkerTask != nullptr) {
        Timing::cancelTimer(checkerTask);
        checkerTask = nullptr;
    }
}

void TriggeredMovement::update() {
    MovementStatus currentStatus = status->get();

    if (currentStatus == MovementStatus::IDLE || currentStatus == MovementStatus::SET) return;
    
    status->reset();

    checkAndExecuteDeps();

    if (!status->hasChanged()) return;
    
    callback(target->get());
    
    if (checkerTask != nullptr) {
        Timing::cancelTimer(checkerTask);
        checkerTask = nullptr;
    }

    checkerTask = Timing::when(
        [](void* pSelf)->bool{
            TriggeredMovement* movement = static_cast<TriggeredMovement*>(pSelf);

            return movement->checker(movement->target->get());
        },
    [](void* pSelf) {
        TriggeredMovement* movement = static_cast<TriggeredMovement*>(pSelf);
        
        movement->status->set(MovementStatus::SET);
    }, this, this);

    return;
}