#pragma once

#include <DFRobot_RGBLCD1602.h>

class LCD {
    public:
    static void setup();
    static void update();

    static void display(const char* str);

    private:
    
    static DFRobot_RGBLCD1602 screen;
};