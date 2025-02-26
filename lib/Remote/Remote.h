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

    Remote();

    RF24 radio;

    BaseTimer* timeoutTimer;

    // Singleton stuff
    Remote(const Remote&) = delete;
    Remote& operator=(const Remote&) = delete;
    
    public:
    
    void setup();
    bool fetch(RemoteData& dataBuffer);

    static Remote& getInstance();
};