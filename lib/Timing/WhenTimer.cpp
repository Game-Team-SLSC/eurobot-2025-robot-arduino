#include <WhenTimer.h>

WhenTimer::WhenTimer(bool(*checker)(void*), void(*callback)(void*), void* checkerArg, void* arg) :
BaseTimer(0, arg),
checker(checker),
callback(callback),
checkerArg(checkerArg)
{};

void WhenTimer::update() {
    if (!checker(checkerArg)) return;

    callback(arg);
}