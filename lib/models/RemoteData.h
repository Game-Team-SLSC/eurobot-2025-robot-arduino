#pragma once

#include <Arduino.h>
#include <JoystickData.h>

enum SWITCH_3_POS {UP, DOWN, MIDDLE};

struct RemoteData
{
    JoystickData joystickLeft;
    JoystickData joystickRight;

    bool buttons[10]; // for each button true if pressed
    byte slider; // -128 to 127
    int8_t score; // 0 to 255
    SWITCH_3_POS sw; // 3 position switch
};