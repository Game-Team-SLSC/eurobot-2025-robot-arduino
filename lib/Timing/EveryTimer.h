#pragma once

#include <BaseTimer.h>

class EveryTimer: public BaseTimer {
    public:
    
    EveryTimer(unsigned int interval, bool(*callback)(void*), void* arg);
    ~EveryTimer() = default;
    void update() override;

    private:

    bool(*callback)(void*);
    unsigned long lastTick;
};