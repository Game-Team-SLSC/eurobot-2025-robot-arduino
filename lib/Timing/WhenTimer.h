#pragma once

#include <BaseTimer.h>

class WhenTimer: public BaseTimer {
    public:
    
    WhenTimer(bool(*checker)(void*), void(*callback)(void*), void* checkerArg = nullptr, void* arg = nullptr);
    ~WhenTimer() = default;
    void update() override;

    private:

    bool(*checker)(void*);
    void(*callback)(void*);

    void* checkerArg;
};