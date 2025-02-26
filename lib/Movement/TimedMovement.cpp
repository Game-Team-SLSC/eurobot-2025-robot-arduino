#include <TimedMovement.h>

TimedMovement::TimedMovement(MovementCallback callback, unsigned int duration) :
Movement(callback),
duration(duration),
timer(nullptr)
{}

void TimedMovement::update() {
    MovementStatus currentStatus = status->get();

    if (currentStatus == MovementStatus::IDLE || currentStatus == MovementStatus::SET) return;
    
    status->reset();
    checkAndExecuteDeps();
    if (!status->hasChanged()) return;

    if (currentStatus == MovementStatus::MOVING) {
        callback(target->get());
        
        if (timer != nullptr) {
            Timing::cancelTimer(timer);
            timer = nullptr;
        }
    
        timer = Timing::in(duration, +[](void* pSelf) {
            TimedMovement* movement = static_cast<TimedMovement*>(pSelf);
            
            movement->status->set(MovementStatus::SET);
        }, this);

        return;
    }
}