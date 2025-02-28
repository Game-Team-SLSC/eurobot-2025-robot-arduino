#pragma once

#include <Arduino.h>
#include <JoystickData.h>

enum SWITCH_3_POS {UP, DOWN, MIDDLE};

struct RemoteData
{
    JoystickData joystickLeft;
    JoystickData joystickRight;

    bool buttons[10]; // for each button true if pressed
    byte slider; // 0 to 255
    byte score; // 0 to 255
    SWITCH_3_POS sw; // 3 position switch
};