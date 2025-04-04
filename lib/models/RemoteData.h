#pragma once

#include <Arduino.h>
#include <JoystickData.h>
#include <GlobalSettings.h>

struct RemoteData
{
    JoystickData joystickLeft;
    JoystickData joystickRight;

    bool buttons[_BUTCOUNT] = {
        false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false
    }; // for each button true if pressed
    byte slider = 255; // 0 to 255
    byte score = DEFAULT_SCORE; // 0 to 255
};