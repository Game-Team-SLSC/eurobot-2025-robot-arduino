#pragma once

#include <Action.h>
#include <Movement.h>
#include <TimedMovement.h>
#include <TriggeredMovement.h>
#include <Arduino.h>
#include <TMCStepper.h>
#include <AccelStepper.h>
#include <Adafruit_PWMServoDriver.h>
#include <MovementName.h>
#include <ActionName.h>

class Actuators {
    public:

    static void setup();
    static void update();
    
    private:
    
    static Movement* movements[_MOVCOUNT];
    static Action* actions[_ACTCOUNT];

    static Adafruit_PWMServoDriver pwmDriver;

    static TMC2209Stepper grbDriver;
    static AccelStepper grbStepper;

    static TMC2209Stepper sucDriver;
    static AccelStepper sucStepper;

    // movements

    static void enablePump(bool enable);
    static void attachMagnet(bool attach);
    static void deployArm(bool deploy);
    static void catchBlock(bool _catch);
    static void deploySuction(bool deploy);
    static void moveGrabber(bool up);
    static void moveSuction(bool up);
    static void releaseBanner(bool up);

    static bool isGrabberBlockMoved(bool up);
    static bool isSuctionBlockMoved(bool up);

    static void setServoAngle(uint8_t num, byte angle);
};