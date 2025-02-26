#include <BaseTimer.h> 

BaseTimer::BaseTimer(int delay, void* arg):
start(millis()),
delay(delay),
arg(arg),
dead(false)
{};

bool BaseTimer::isDead() const {
    return dead;
}