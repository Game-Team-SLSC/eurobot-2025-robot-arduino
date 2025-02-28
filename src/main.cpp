#include <Remote.h>
#include <GlobalState.h>
#include <DynamicStateController.h>
#include <Logger.h>
#include <RemoteData.h>
#include <Timing.h>
#include <ScoreDisplay.h>
#include <Movers.h>
#include <Actuators.h>
#include <Arduino.h>

RemoteData remoteData;

void setup() {
    loggerSetup();
    info("Powered up !");

    info("Radio setup...");
    Remote::setup();

    info("Score display setup...");
    ScoreDisplay::setup();
    
    info("Movers setup...");
    Movers::setup();
    
    info("Actuators setup in 2 seconds. Put the robot in its initial position.");
    delay(2000);
    Actuators::setup();
 
    info("Robot ready !");
}

void postRun() {
    DynamicStateController::getInstance().resetStates();
}

bool acquire() {
    // for tests

    // remoteData = {
    //      {0, 0, false}, // joystickLeft
    //      {0, 0, false}, // joystickRight
    //      {false, false, false, false, false, false, false, false, false, false}, // buttons
    //      255, // slider
    //      0, // score
    //      MIDDLE // sw
    // };
    // GlobalState::remoteConnected->set(true);
    //
    //return true;


    return Remote::fetch(remoteData);
}

void process() {
    GlobalState::updateFromController(remoteData);
}

void heartbeat() {
    Timing::update();
    Movers::update();
    ScoreDisplay::update();
    Actuators::update();
}

void loop() {
    delay(LOOP_DELAY);
    if (acquire()) {
        process();
    }
    heartbeat();
    postRun();
}