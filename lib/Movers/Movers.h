#pragma once

#include <L298NX2.h>
#include <GlobalSettings.h>
#include <GlobalState.h>

class Movers {
    public:

    static void setup();
    static void update();

    private:

    static L298NX2 frontDriver;
    static L298NX2 rearDriver;
};