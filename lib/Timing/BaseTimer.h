#pragma once

#include <Arduino.h>

class BaseTimer {
    public:

    virtual ~BaseTimer() = default;
    
    virtual void update() = 0;
    bool isDead() const;
    
    protected:

    BaseTimer(int delay, void* arg);
    
    unsigned long start;
    unsigned int delay;
    void* arg;
    bool dead;
};