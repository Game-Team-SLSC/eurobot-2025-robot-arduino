#include <Remote.h>
#include <GlobalState.h>
#include <DynamicStateController.h>
#include <Logger.h>
#include <RemoteData.h>
#include <Timing.h>
#include <ScoreDisplay.h>
#include <Actuators.h>

RemoteData remoteData;

void setup() {
    loggerSetup();
    info("Powered up !");

    info("Radio setup...");
    Remote::getInstance().setup();

    info("Score display setup...");
    ScoreDisplay::setup();

    info("Actuators setup in 2 seconds. Put the robot in its initial position.");
    delay(2000);
    Actuators::setup();
 
    info("Robot ready !");
}

void preRun() {
    DynamicStateController::getInstance().resetStates();
}

bool acquire() {
    remoteData = {
        {128, 128, false}, // joystickLeft
        {128, 128, false}, // joystickRight
        {false, false, false, false, false, false, false, false, false, false}, // buttons
        128, // slider
        0, // score
        MIDDLE // sw
    };
    
    return true;
    //return Remote::getInstance().fetch(remoteData);
}

void process() {
    GlobalState::updateFromController(remoteData);
}

void heartbeat() {
    Timing::update();
    ScoreDisplay::update();
    Actuators::update();
}

void loop() {
    preRun();
    if (acquire()) {
        process();
    }
    heartbeat();
}