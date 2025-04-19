#include <Remote.h>
#include <GlobalState.h>
#include <GlobalSettings.h>
#include <DynamicStateController.h>
#include <Logger.h>
#include <RemoteData.h>
#include <Timing.h>
#include <ScoreDisplay.h>
#include <Movers.h>
#include <Actuators.h>
#include <Auto.h>
#include <Arduino.h>
#include <LCD.h>

RemoteData remoteData;
RemoteData emulatedData;

long lastMillis;

void setup() {
    loggerSetup();
    info("Powered up !");

    #ifdef MOCK_REMOTE
    warn("MOCK_REMOTE is enabled. Remote data will be emulated.");
    #else
    info("Radio setup...");
    Remote::setup();
    #endif

    info("LCD setup...");
    LCD::setup();

    info("Score display setup...");
    ScoreDisplay::setup();
    
    info("Movers setup...");
    Movers::setup();
    
    info("Actuators setup in 1 second. Put the robot in its initial position.");
    delay(1000);
    Actuators::setup();
 
    info("Robot ready !");
    LCD::display("Robot ready !");
    
    lastMillis = millis();
}

void preRun() {
    Timing::updatePreRun();
}

void postRun() {
    DynamicStateController::getInstance().resetStates();
}

bool acquire() {
    #ifdef MOCK_REMOTE
    bool remoteSuccess = true;
    GlobalState::remoteConnected->set(true);
    #else
    bool remoteSuccess = Remote::fetch(remoteData);
    #endif

    Auto::fetchData(emulatedData);
    
    return remoteSuccess;
}

void process() {
    GlobalState::update(remoteData, emulatedData);
}

void heartbeat() {
    Timing::update();
    Movers::update();
    ScoreDisplay::update();
    Actuators::update();
    LCD::update();
}

void loop() {
    preRun();
    acquire();
    process();
    heartbeat();
    postRun();
}