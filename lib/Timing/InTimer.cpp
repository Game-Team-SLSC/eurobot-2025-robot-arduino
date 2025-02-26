#include <InTimer.h>

InTimer::InTimer(unsigned int delay, void(*callback)(void*), void* arg):
BaseTimer(delay, arg),
callback(callback)
{}

void InTimer::update() {
    if (millis() - start < delay) return;

    callback(arg);
    dead = true;
}