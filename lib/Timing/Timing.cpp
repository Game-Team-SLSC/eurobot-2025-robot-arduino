#include <Timing.h>

BaseTimer* Timing::timers[MAX_TIMERS] = {nullptr};
BaseTimer* Timing::preRunTimers[MAX_PRE_RUN_TIMERS] = {nullptr};

InTimer* Timing::in(unsigned int delay, void(*callback)(void*), void* arg, bool isPreRun) {
    InTimer* timer = new InTimer(delay, callback, arg);

    byte index = getTimerCount();

    if (index >= MAX_TIMERS) {
        error("Timer stack overflow");
        return nullptr;
    }

    if (isPreRun) preRunTimers[index] = timer;
    else timers[index] = timer;

    return timer;
}

EveryTimer* Timing::every(unsigned int interval, bool(*callback)(void*), void* arg, bool isPreRun) {
    EveryTimer* timer = new EveryTimer(interval, callback, arg);

    byte index = getTimerCount();

    if (index >= MAX_TIMERS) {
        error("Timer stack overflow");
        return nullptr;
    }

    if (isPreRun) preRunTimers[index] = timer;
    else timers[index] = timer;

    return timer;
}

WhenTimer* Timing::when(bool(*checker)(void*), void(*callback)(void*), void* checkerArg, void* arg, bool isPreRun) {
    WhenTimer* timer = new WhenTimer(checker, callback, checkerArg, arg);

    byte index = getTimerCount();

    if (index >= MAX_TIMERS) {
        error("Timer stack overflow");
        return nullptr;
    }

    if (isPreRun) preRunTimers[index] = timer;
    else timers[index] = timer;

    return timer;
}

void Timing::cancelTimer(BaseTimer* timer) {
    byte newElemIndex = getTimerCount();

    for (byte i = 0; i < newElemIndex; ++i) {
        if (timers[i] != timer) continue;

        deleteTimer(timer, i);
        break;
    }
}

void Timing::updatePreRun() {
    byte newElemIndex = getPreRunTimerCount();

    for (int i = 0; i < newElemIndex; ++i) {
        BaseTimer* timer = preRunTimers[i];

        timer->update();

        if (!timer->isDead()) continue;
        
        deleteTimer(timer, i);

        --newElemIndex;
        --i;
    }
}

void Timing::update() {
    byte newElemIndex = getTimerCount();

    for (int i = 0; i < newElemIndex; ++i) {
        BaseTimer* timer = timers[i];

        timer->update();

        if (!timer->isDead()) continue;
        
        deleteTimer(timer, i);

        --newElemIndex;
        --i;
    }
}

void Timing::deleteTimer(BaseTimer* timer, byte index) {
    byte newElemIndex = getTimerCount();
    
    delete timer;

    for (byte j = index; j < newElemIndex - 1; ++j) {
        timers[j] = timers[j + 1];
    }

    timers[newElemIndex - 1] = nullptr;
}

byte Timing::getTimerCount() {
    byte newElemIndex = 0;
    while (newElemIndex < MAX_TIMERS && timers[newElemIndex] != nullptr) {
        ++newElemIndex;
    }

    return newElemIndex;
}

byte Timing::getPreRunTimerCount() {
    byte newElemIndex = 0;
    while (newElemIndex < MAX_PRE_RUN_TIMERS && preRunTimers[newElemIndex] != nullptr) {
        ++newElemIndex;
    }

    return newElemIndex;
}