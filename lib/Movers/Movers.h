#pragma once

#include <SoftwareSerial.h>
#include <SabertoothSimplified.h>
#include <GlobalSettings.h>
#include <GlobalState.h>

class Movers {
    public:

    static void setup();
    static void update();
    static void stop();

    private:

    static int8_t lastFR;
    static int8_t lastFL;
    static int8_t lastRR;
    static int8_t lastRL;

    static int8_t lerp(float a, float b, float t);

    static SabertoothSimplified leftSB;
    static SabertoothSimplified rightSB;
    static SoftwareSerial leftSerial;
    static SoftwareSerial rightSerial;
};