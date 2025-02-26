#pragma once

#include <BaseTimer.h>

class InTimer: public BaseTimer {
    public:
    
    InTimer(unsigned int delay, void(*callback)(void*), void* arg);
    ~InTimer() = default;
    void update() override;

    private:

    void(*callback)(void*);
};