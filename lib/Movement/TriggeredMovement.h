#pragma once

#include <Movement.h>
#include <Timing.h>

typedef bool(*MovementChecker)(bool active);
class TriggeredMovement: public Movement {
    public:

    // Const/Destructor
    TriggeredMovement(MovementCallback callback, MovementChecker checker);
    ~TriggeredMovement() = default;

    // Functions
    void exec(bool active) override;
    void update() override;

    private:

    // Vars
    MovementChecker checker;
    WhenTimer* checkerTask;
};