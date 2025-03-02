#include <Timing.h>

BaseTimer* Timing::timers[MAX_TIMERS] = {nullptr};
BaseTimer* Timing::preRunTimers[MAX_PRE_RUN_TIMERS] = {nullptr};
byte Timing::timersCount = 0;
byte Timing::preRunTimersCount = 0;

InTimer* Timing::in(unsigned int delay, void(*callback)(void*), void* arg, bool isPreRun) {
    InTimer* timer = new InTimer(delay, callback, arg);

    if (isPreRun) {
        if (preRunTimersCount >= MAX_PRE_RUN_TIMERS) {
            error("Pre-run timer stack overflow");
            return nullptr;
        }
        preRunTimers[preRunTimersCount] = timer;
        preRunTimersCount++;
    }
    else {
        if (timersCount >= MAX_TIMERS) {
            error("Pre run Timer stack overflow");
            return nullptr;
        }
        timers[timersCount] = timer;
        timersCount++;
    }

    return timer;
}

EveryTimer* Timing::every(unsigned int interval, bool(*callback)(void*), void* arg, bool isPreRun) {
    EveryTimer* timer = new EveryTimer(interval, callback, arg);

    if (isPreRun) {
        if (preRunTimersCount >= MAX_PRE_RUN_TIMERS) {
            error("Pre-run timer stack overflow");
            return nullptr;
        }
        preRunTimers[preRunTimersCount] = timer;
        preRunTimersCount++;
    }
    else {
        if (timersCount >= MAX_TIMERS) {
            error("Pre run Timer stack overflow");
            return nullptr;
        }
        timers[timersCount] = timer;
        timersCount++;
    }

    return timer;
}

WhenTimer* Timing::when(bool(*checker)(void*), void(*callback)(void*), void* checkerArg, void* arg, bool isPreRun) {
    WhenTimer* timer = new WhenTimer(checker, callback, checkerArg, arg);

    if (isPreRun) {
        if (preRunTimersCount >= MAX_PRE_RUN_TIMERS) {
            error("Pre-run timer stack overflow");
            return nullptr;
        }
        preRunTimers[preRunTimersCount] = timer;
        preRunTimersCount++;
    }
    else {
        if (timersCount >= MAX_TIMERS) {
            error("Timers stack overflow");
            return nullptr;
        }
        timers[timersCount] = timer;
        timersCount++;
    }

    return timer;
}

void Timing::cancelTimer(BaseTimer* timer) {
    for (byte i = 0; i < timersCount; ++i) {
        if (timers[i] != timer) continue;

        deleteTimer(timer, i);

        i--;
        break;
    }
}

void Timing::cancelPreRunTimer(BaseTimer* timer) {
    for (byte i = 0; i < preRunTimersCount; ++i) {
        if (preRunTimers[i] != timer) continue;

        deletePreRunTimer(timer, i);
        break;
    }
}

void Timing::updatePreRun() {
    for (int i = 0; i < preRunTimersCount; ++i) {
        BaseTimer* timer = preRunTimers[i];

        timer->update();

        if (!timer->isDead()) continue;
        
        deletePreRunTimer(timer, i);
    }
}

void Timing::update() {
    for (int i = 0; i < timersCount; ++i) {
        BaseTimer* timer = timers[i];

        timer->update();

        if (!timer->isDead()) continue;
        
        deleteTimer(timer, i);
    }
}

void Timing::deleteTimer(BaseTimer* timer, byte index) {
    delete timer;

    for (byte j = index; j < timersCount - 1; ++j) {
        timers[j] = timers[j + 1];
    }

    timers[timersCount - 1] = nullptr;
    timersCount--;
}

void Timing::deletePreRunTimer(BaseTimer* timer, byte index) {
    delete timer;

    for (byte j = index; j < preRunTimersCount - 1; ++j) {
        preRunTimers[j] = preRunTimers[j + 1];
    }

    preRunTimers[preRunTimersCount - 1] = nullptr;
    preRunTimersCount--;
}