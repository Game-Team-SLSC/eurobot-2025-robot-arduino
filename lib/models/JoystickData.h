#pragma once

#include <Arduino.h>

struct JoystickData {
    byte x = 128; // 0 to 255
    byte y = 128; // 0 to 255
};