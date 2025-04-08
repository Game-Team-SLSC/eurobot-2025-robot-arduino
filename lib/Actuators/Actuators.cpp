#include "Actuators.h"
#include <GlobalSettings.h>
#include <GlobalState.h>
#include <Wire.h>

TMC2209Stepper Actuators::driverL = TMC2209Stepper(&PT_SERIAL_L, R_SENSE, 0b00);
TMC2209Stepper Actuators::driverR = TMC2209Stepper(&PT_SERIAL_R, R_SENSE, 0b00);
Adafruit_PWMServoDriver Actuators::pwmDriver = Adafruit_PWMServoDriver(0x40);
AccelStepper Actuators::stepperL = AccelStepper(AccelStepper::DRIVER, PT_STEP_L, PT_DIR_L);
AccelStepper Actuators::stepperR = AccelStepper(AccelStepper::DRIVER, PT_STEP_R, PT_DIR_R);
MultiStepper Actuators::steppers = MultiStepper();
VL53L0X Actuators::distanceSensor = VL53L0X();

Movement* Actuators::movements[__MOV_COUNT] = {};
Actuator* Actuators::actuators[__ACTUATOR_COUNT] = {};
Action* Actuators::actions[__ACTION_COUNT] = {};
MovementDependency* Actuators::stepsBuffer[STEP_BUFFER_SIZE];
byte Actuators::stepsCount = 0;
bool Actuators::actionRunning = false;

void Actuators::setup() {
    setupHardware();
    setupMovements();
    setupActuators();
    setupActions();
}

void Actuators::update() {
    if (GlobalState::action->hasChanged()) startAction();

    if (!actionRunning) return;

    updateAction();

}

bool Actuators::isActionRunning() {
    return actionRunning;
}

void Actuators::startAction() {
    info("Starting action %d", GlobalState::action->get());
    actionRunning = true;

    // reset buffer
    for (byte i = 0; i < STEP_BUFFER_SIZE; i++) {
        stepsBuffer[i] = nullptr;
    }

    // make buffer
    Action* action = actions[GlobalState::action->get()];
    
    for (byte i = 0; i < action->stepCount; i++) {
        addStepToBuffer(&action->steps[i]);
    }

    warn("Did start action");
}

void Actuators::updateAction() {
    for(;;) {
        if (stepsCount == 0) {
            info("Action %d finished !", GlobalState::action->get());
            actionRunning = false;
            return;
        }
        
        MovementDependency* pStep = stepsBuffer[0];
        Movement* pMovement = movements[pStep->movement];
        Actuator* pActuator = getActuatorFromMovement(pStep->movement);

        if (pActuator->targetMovement != pStep->movement || pActuator->status == ActuatorStatus::IDLE) {
            execStep(pStep);
        }

        if (pMovement->getType() == TRIGGERED) {
            TriggeredMovement* pTriggeredMovement = static_cast<TriggeredMovement*>(pMovement);
            if (pTriggeredMovement->checker() || (millis() - pTriggeredMovement->startTime) >= pTriggeredMovement->timeout) {
                pActuator->status = SET;
            }
        } else {
            TimedMovement* pTimedMovement = static_cast<TimedMovement*>(pMovement);
            if (millis() - pTimedMovement->startTime >= pTimedMovement->duration) {
                pActuator->status = SET;
            }
        }

        if (pStep->desiredStatus == MOVING || pActuator->status == pStep->desiredStatus) {
            stepsCount--;

            pActuator->status = SET; // TODO: Add an update buffer but for now, we assume the actuator is set if we re just waiting for moving 

            for (byte i = 0; i < stepsCount; i++) {
                stepsBuffer[i] = stepsBuffer[i + 1];
            }


            stepsBuffer[stepsCount] = nullptr;

        } else return;
    }
}

void Actuators::addStepToBuffer(MovementDependency* pStep) {
    Movement* pMovement = movements[pStep->movement];

    // Check if dependencies exist before accessing them
    if (pMovement->dependencies != nullptr && pMovement->dependencyCount > 0) {
        for (byte i = 0; i < pMovement->dependencyCount; i++) {
            Actuators::addStepToBuffer(&pMovement->dependencies[i]);
        }
    }

    stepsBuffer[stepsCount] = pStep;
    stepsCount++;
}

void Actuators::setupHardware() {
    Wire.begin();

    pwmDriver.begin();
    pwmDriver.setOscillatorFrequency(27000000);
    pwmDriver.setPWMFreq(50);

    pinMode(PT_DIR_L, OUTPUT);
    pinMode(PT_STEP_L, OUTPUT);

    driverL.begin();
    driverL.rms_current(900);
    driverL.pwm_autoscale(true);
    driverL.microsteps(1);

    pinMode(PT_EN_L, OUTPUT);
    digitalWrite(PT_EN_L, LOW);
    stepperL.setPinsInverted(false, false, true);
    stepperL.setMaxSpeed(10000);
    stepperL.setAcceleration(20000);
    stepperL.enableOutputs();

    stepperL.setCurrentPosition(0);
    
    pinMode(PT_DIR_R, OUTPUT);
    pinMode(PT_STEP_R, OUTPUT);

    driverR.begin();
    driverR.rms_current(900);
    driverR.microsteps(1);
    driverR.pwm_autoscale(true);

    pinMode(PT_EN_R, OUTPUT);
    digitalWrite(PT_EN_R, LOW);
    stepperR.setPinsInverted(false, false, true);
    stepperR.setMaxSpeed(10000);
    stepperR.setAcceleration(20000);
    stepperR.enableOutputs();
    
    stepperR.setCurrentPosition(0);

    steppers.addStepper(stepperL);
    steppers.addStepper(stepperR);

    //distanceSensor.startContinuous();
}

void Actuators::setupMovements() {
    static MovementDependency magnetAttachDeps[] = {
        {ARM_DEPLOY, ActuatorStatus::SET}
    };
    movements[MAGNET_ATTACH] = new TimedMovement(
        []() {
            setServoAngle(GRB_MAGNET_L_PIN, GRB_MAGNET_ATTACH_ANGLE_L);
            setServoAngle(GRB_MAGNET_R_PIN, GRB_MAGNET_ATTACH_ANGLE_R);
        },
        500,
        magnetAttachDeps,
        1
    );

    static MovementDependency magnetDetachDeps[] = {
        {ARM_DEPLOY, ActuatorStatus::SET}
    };
    movements[MAGNET_DETACH] = new TimedMovement(
        []() {
            setServoAngle(GRB_MAGNET_L_PIN, GRB_MAGNET_RELEASE_ANGLE_L);
            setServoAngle(GRB_MAGNET_R_PIN, GRB_MAGNET_RELEASE_ANGLE_R);
        },
        500,
        magnetDetachDeps,
        1
    );

    movements[ARM_DEPLOY] = new TimedMovement(
        []() {
            setServoAngle(GRB_ARM_L_PIN, GRB_ARM_DEP_ANGLE_L);
            setServoAngle(GRB_ARM_R_PIN, GRB_ARM_DEP_ANGLE_R);
        },
        500,
        nullptr,
        0
    );

    static MovementDependency armRetractDeps[] = {
        {MAGNET_ATTACH, ActuatorStatus::SET}
    };
    movements[ARM_RETRACT] = new TimedMovement(
        []() {
            setServoAngle(GRB_ARM_L_PIN, GRB_ARM_RET_ANGLE_L);
            setServoAngle(GRB_ARM_R_PIN, GRB_ARM_RET_ANGLE_R);
        },
        500,
        armRetractDeps,
        2
    );

    movements[GRABBER_CATCH] = new TimedMovement(
        []() {
            setServoAngle(GRB_L_PIN, GRB_CATCH_ANGLE_L);
            setServoAngle(GRB_R_PIN, GRB_CATCH_ANGLE_R);
        },
        500,
        nullptr,
        0
    );

    movements[GRABBER_RELEASE] = new TimedMovement(
        []() {
            setServoAngle(GRB_L_PIN, GRB_RELEASE_ANGLE_L);
            setServoAngle(GRB_R_PIN, GRB_RELEASE_ANGLE_R);
        },
        1000,
        nullptr,
        0
    );

    movements[SUCTION_APPLY] = new TimedMovement(
        []() {
            // TODO
        },
        1000,
        nullptr,
        0
    );

    movements[SUCTION_LIFT] = new TimedMovement(
        []() {
            // TODO
        },
        1000,
        nullptr,
        0
    );

    static MovementDependency sucRetractDeps[] = {
        {PUMP_DISABLE, ActuatorStatus::SET}
    };
    movements[SUCTION_RETRACT] = new TimedMovement(
        []() {
            // TODO
        },
        1000,
        sucRetractDeps,
        1
    );
    movements[PLATFORM_UP] = new TriggeredMovement(
        []() {
            long pos[2];
            pos[0] = PLATFROM_HEIGHT * STEPS_PER_MM;
            pos[1] = -PLATFROM_HEIGHT * STEPS_PER_MM;
            
            stepperL.moveTo(pos[0]);
            stepperR.moveTo(pos[1]);

            while (stepperL.distanceToGo() != 0 || stepperR.distanceToGo() != 0) {
                stepperL.run();
                stepperR.run(); 
            }
        },
        []() {
            return true;
        },
        2000,
        nullptr,
        0
    );

    movements[PLATFORM_DOWN] = new TriggeredMovement(
        []() {
            long pos[2];
            pos[0] = 0;
            pos[1] = 0;
            
            stepperL.moveTo(pos[0]);
            stepperR.moveTo(pos[1]);

            steppers.runSpeedToPosition();
        },
        []() {
            return true;
        },
        2000,
        nullptr,
        0
    );

    static MovementDependency sucBlockApplyDeps[] = {
        {SUCTION_APPLY, ActuatorStatus::SET}  
    };

    movements[BANNER_RELEASE] = new TimedMovement(
        []() {
            //setServoAngle(BANNER_PIN, BANNER_DEP_ANGLE);
        },
        1000,
        nullptr,
        0
    );

    movements[BANNER_CATCH] = new TimedMovement(
        []() {
            //setServoAngle(BANNER_PIN, BANNER_RET_ANGLE);
        },
        1000,
        nullptr,
        0
    );

    movements[PUMP_ENABLE] = new TimedMovement(
        []() {
            digitalWrite(PUMP_RLY, HIGH);
        },
        500,
        nullptr,
        0
    );

    movements[PUMP_DISABLE] = new TimedMovement(
        []() {
            digitalWrite(PUMP_RLY, LOW);
        },
        500,
        nullptr,
        0
    );
}

void Actuators::setupActuators() {
    static MovementName magnetMoves[] = {
        MAGNET_ATTACH,
        MAGNET_DETACH
    };
    actuators[MAGNET] = new Actuator(magnetMoves, sizeof(magnetMoves) / sizeof(MovementName));

    static MovementName armMoves[] = {
        ARM_DEPLOY,
        ARM_RETRACT
    };
    actuators[ARM] = new Actuator(armMoves, sizeof(armMoves) / sizeof(MovementName));

    static MovementName grabberMoves[] = {
        GRABBER_CATCH,
        GRABBER_RELEASE
    };
    actuators[GRABBER] = new Actuator(grabberMoves, sizeof(grabberMoves) / sizeof(MovementName));

    static MovementName suctionMoves[] = {
        SUCTION_APPLY,
        SUCTION_LIFT,
        SUCTION_RETRACT
    };
    actuators[SUCTION] = new Actuator(suctionMoves, sizeof(suctionMoves) / sizeof(MovementName));

    static MovementName platfromMoves[] = {
        PLATFORM_UP,
        PLATFORM_DOWN
    };
    actuators[PLATFORM] = new Actuator(platfromMoves, sizeof(platfromMoves) / sizeof(MovementName));

    static MovementName bannerMoves[] = {
        BANNER_RELEASE,
        BANNER_CATCH
    };
    actuators[BANNER] = new Actuator(bannerMoves, sizeof(bannerMoves) / sizeof(MovementName));

    static MovementName pumpMoves[] = {
        PUMP_ENABLE,
        PUMP_DISABLE
    };
    actuators[PUMP] = new Actuator(pumpMoves, sizeof(pumpMoves) / sizeof(MovementName));
}

void Actuators::setupActions() {
    static MovementDependency foldSteps[] = {
        {BANNER_CATCH, ActuatorStatus::MOVING},
        {PLATFORM_UP, ActuatorStatus::SET},
        {ARM_RETRACT, ActuatorStatus::MOVING},
        {GRABBER_CATCH, ActuatorStatus::MOVING},
        {SUCTION_RETRACT, ActuatorStatus::MOVING},
    };
    actions[FOLD] = new Action(foldSteps, sizeof(foldSteps) / sizeof(MovementDependency));

    static MovementDependency approachSteps[] = {
        {ARM_DEPLOY, ActuatorStatus::MOVING},
        {MAGNET_ATTACH, ActuatorStatus::MOVING},
        {GRABBER_RELEASE, ActuatorStatus::MOVING},
        {SUCTION_RETRACT, ActuatorStatus::MOVING},
        {PLATFORM_DOWN, ActuatorStatus::SET},
    };
    actions[APPROACH] = new Action(approachSteps, sizeof(approachSteps) / sizeof(MovementDependency));

    static MovementDependency transportSteps[] = {
        {ARM_DEPLOY, ActuatorStatus::MOVING},
        {MAGNET_ATTACH, ActuatorStatus::MOVING},
        {GRABBER_CATCH, ActuatorStatus::MOVING},
        {SUCTION_APPLY, ActuatorStatus::SET},
    };
    actions[TRANSPORT] = new Action(transportSteps, sizeof(transportSteps) / sizeof(MovementDependency));

    static MovementDependency releaseSteps[] = {
        {PUMP_DISABLE, ActuatorStatus::MOVING},
        {MAGNET_DETACH, ActuatorStatus::SET},
    };
    actions[RELEASE] = new Action(releaseSteps, sizeof(releaseSteps) / sizeof(MovementDependency));

    static MovementDependency extractStageSteps[] = {
        // Secure catch
        {GRABBER_CATCH, ActuatorStatus::MOVING},
        {SUCTION_APPLY, ActuatorStatus::SET},
        
        // Extraction
        {ARM_RETRACT, ActuatorStatus::MOVING},
        {PUMP_ENABLE, ActuatorStatus::SET},
        {SUCTION_LIFT, ActuatorStatus::MOVING},

        // Release
        {GRABBER_RELEASE, ActuatorStatus::SET},
    };
    actions[EXTRACT_STAGE] = new Action(extractStageSteps, sizeof(extractStageSteps) / sizeof(MovementDependency));

    static MovementDependency bannerSteps[] = {
        {BANNER_RELEASE, ActuatorStatus::SET},
    };
    actions[BANNER_DEPLOY] = new Action(bannerSteps, sizeof(bannerSteps) / sizeof(MovementDependency));

    static MovementDependency s1Steps[] = {
        {PLATFORM_DOWN, ActuatorStatus::SET},
    };
    actions[S1] = new Action(s1Steps, sizeof(s1Steps) / sizeof(MovementDependency));

    static MovementDependency s2Steps[] = {
        {PLATFORM_UP, ActuatorStatus::SET},
    };
    actions[S2] = new Action(s2Steps, sizeof(s2Steps) / sizeof(MovementDependency));

    static MovementDependency catch2SSteps[] = {
        {GRABBER_CATCH, ActuatorStatus::SET}
    };
    actions[CATCH_2S] = new Action(catch2SSteps, sizeof(catch2SSteps) / sizeof(MovementDependency));
}

void Actuators::setServoAngle(byte pin, byte angle) {
    pwmDriver.writeMicroseconds(pin, map(angle, 0, 180, 400, 2850));
}

Actuator* Actuators::getActuatorFromMovement(MovementName movement) {
    for (byte i = 0; i < __ACTUATOR_COUNT; i++) {
        Actuator* pActuator = actuators[i];
        for (byte j = 0; j < pActuator->moveCount; j++) {
            if (pActuator->moves[j] == movement) {
                return pActuator;
            }
        }
    }

    return nullptr;
}

void Actuators::execStep(MovementDependency* pStep) {
    Movement* pMovement = movements[pStep->movement];
    Actuator* pActuator = getActuatorFromMovement(pStep->movement);

    info("Executing action step %d", pStep->movement);

    pActuator->targetMovement = pStep->movement;
    pMovement->startTime = millis();

    pActuator->status = ActuatorStatus::MOVING;
    pMovement->callback();
}