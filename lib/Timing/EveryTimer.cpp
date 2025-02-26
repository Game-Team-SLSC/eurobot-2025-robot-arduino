#include <EveryTimer.h>

EveryTimer::EveryTimer(unsigned int interval, bool(*callback)(void*), void* arg):
BaseTimer(interval, arg),
callback(callback),
lastTick(start)
{}

void EveryTimer::update() {
    unsigned long tick = millis();
    if (tick - lastTick < delay) return;

    lastTick = tick;
    if (!callback(arg)) {
        dead = true;
    }
}