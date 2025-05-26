#pragma once

#include <RemoteData.h>
#include <AutoStep.h>

#define MAX_STEPS 15

class Auto {
    public:

    static void setup();
    static void fetchData(RemoteData& dataBuffer);

    private:

    static RemoteData* emulatedData;
    static AutoStep* stepsBuffer[MAX_STEPS];
    static byte stepCount;
    static bool isRunning;
    static byte lastRunMode;

    static void pressButton(Buttons button);
    static void setJoysticks(int8_t x, int8_t y, int8_t z);

    static void clearBuffer();
    static void execMode();
    static void execStep();
    static void resetData();

    static void exec2Stages();
    static void exec3Stages();
    static void execRelease3Stages();
};