#pragma once

#include <SoftwareSerial.h>
#include <SabertoothSimplified.h>
#include <GlobalSettings.h>
#include <GlobalState.h>

class Movers {
    public:

    static void setup();
    static void update();

    private:

    static int8_t lastFR;
    static int8_t lastFL;
    static int8_t lastRR;
    static int8_t lastRL;

    static int8_t lerp(int8_t a, int8_t b, float t);

    static SabertoothSimplified leftSB;
    static SabertoothSimplified rightSB;
    static SoftwareSerial leftSerial;
    static SoftwareSerial rightSerial;
};