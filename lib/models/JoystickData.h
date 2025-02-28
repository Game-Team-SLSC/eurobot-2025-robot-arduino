#pragma once

#include <Arduino.h>

struct JoystickData {
    byte x; // 0 to 255
    byte y; // 0 to 255
    bool sw; // true if pressed

    bool operator==(const JoystickData &other) {
        return x == other.x && y == other.y && sw == other.sw;
    }
};