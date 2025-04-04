#pragma once

#include <Arduino.h>

struct JoystickData {
    byte x = 100; // 0 to 255
    byte y = 128; // 0 to 255
};