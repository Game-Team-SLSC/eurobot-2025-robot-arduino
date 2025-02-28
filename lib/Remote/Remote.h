#pragma once

#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>
#include <RemoteData.h>
#include <GlobalSettings.h>
#include <GlobalState.h>
#include <Timing.h>

class Remote {
    private:

    static RF24 radio;
    static BaseTimer* timeoutTimer;
    
    public:
    
    static void setup();
    static bool fetch(RemoteData& dataBuffer);
};