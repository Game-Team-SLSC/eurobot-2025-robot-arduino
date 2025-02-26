#pragma once

#include <Movement.h>
#include <Timing.h>

class TimedMovement: public Movement {
    public:

    // Const/Destructor
    TimedMovement(MovementCallback callback, unsigned int duration);
    ~TimedMovement() = default;

    // Functions
    void update() override;

    private:

    // Vars
    unsigned int duration;
    InTimer* timer;
};