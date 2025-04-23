#pragma once

#include <Adafruit_PWMServoDriver.h>
#include <VL53L0X.h>

#include "classes/Movement.h"
#include "classes/Actuator.h"
#include "classes/MovementDependency.h"
#include "classes/Action.h"
#include <ActionName.h>
#include "enums/MovementName.h"
#include "enums/ServoType.h"

#define STEP_BUFFER_SIZE 30

class Actuators {
    public:
    
    static void setup();
    static void update();

    static bool isActionRunning();

    private:

    static void setupHardware();
    static void setupMovements();
    static void setupActuators();
    static void setupActions();

    static Actuator* getActuatorFromMovement(MovementName movement);
    
    static void startAction();
    static void updateAction();
    static void execStep(MovementDependency* step);
    static void addStepToBuffer(MovementDependency* step);

    static inline void setServoAngle(byte pin, short angle, ServoType servoType = MG996R);

    static Movement* movements[__MOV_COUNT];
    static Actuator* actuators[__ACTUATOR_COUNT];
    static Action* actions[__ACTION_COUNT];

    static Adafruit_PWMServoDriver pwmDriver;
    static VL53L0X distanceSensor;

    static MovementDependency* stepsBuffer[STEP_BUFFER_SIZE];
    static byte stepsCount;

    static bool actionRunning;
};