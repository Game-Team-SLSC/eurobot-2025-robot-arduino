#pragma once

#include <Arduino.h>
#include <BaseTimer.h>
#include <InTimer.h>
#include <EveryTimer.h>
#include <WhenTimer.h>
#include <Logger.h>

#define MAX_PRE_RUN_TIMERS 10
#define MAX_TIMERS 50

class Timing {
    public:

    static InTimer* in(unsigned int delay, void(*callback)(void*), void* arg = nullptr, bool isPreRun = false);
    static EveryTimer* every(unsigned int interval, bool(*callback)(void*), void* arg = nullptr, bool isPreRun = false);
    static WhenTimer* when(bool(*checker)(void*), void(*callback)(void*), void* checkerArg = nullptr, void* arg = nullptr, bool isPreRun = false);
    
    static void update();
    static void updatePreRun();
    static void cancelTimer(BaseTimer* timer);

    private:

    static void deleteTimer(BaseTimer* timer, byte index);

    static BaseTimer* preRunTimers[MAX_PRE_RUN_TIMERS];
    static BaseTimer* timers[MAX_TIMERS];
    static byte getTimerCount();
    static byte getPreRunTimerCount();
};