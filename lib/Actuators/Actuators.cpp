#include "Actuators.h"
#include <GlobalSettings.h>
#include <GlobalState.h>
#include <Wire.h>
#include <TimerOne.h>

TMC2209Stepper Actuators::grbDriver = TMC2209Stepper(&GRB_SERIAL, R_SENSE, 0b00);
TMC2209Stepper Actuators::sucDriver = TMC2209Stepper(&SC_SERIAL, R_SENSE, 0b00);
Adafruit_PWMServoDriver Actuators::pwmDriver = Adafruit_PWMServoDriver(0x40);
AccelStepper Actuators::grbStepper = AccelStepper(AccelStepper::DRIVER, GRB_STEP, GRB_DIR);
AccelStepper Actuators::sucStepper = AccelStepper(AccelStepper::DRIVER, SC_STEP, SC_DIR);
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
}

void Actuators::updateAction() {
    for(;;) {
        if (stepsCount == 0) {
            info("Action %d finished !", GlobalState::action->get());
            actionRunning = false;
            return;
        }
        
        MovementDependency* pStep = stepsBuffer[0];
        Actuator* pActuator = getActuatorFromMovement(pStep->movement);

        if (pActuator->targetMovement != pStep->movement || pActuator->status == ActuatorStatus::IDLE) {
            execStep(pStep);
        }

        if (pStep->desiredStatus == MOVING || pActuator->status == pStep->desiredStatus) {
            for (byte i = 0; i < stepsCount - 1; i++) {
                stepsBuffer[i] = stepsBuffer[i + 1];
            }

            stepsBuffer[stepsCount - 1] = nullptr;
    
            stepsCount--;
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

    pinMode(GRB_DIR, OUTPUT);
    pinMode(GRB_STEP, OUTPUT);

    grbDriver.begin();
    grbDriver.rms_current(900);
    grbDriver.pwm_autoscale(true);
    grbDriver.microsteps(1);

    grbStepper.setEnablePin(GRB_EN);
    grbStepper.setPinsInverted(false, false, true);
    grbStepper.setMaxSpeed(3500);
    grbStepper.setAcceleration(5000);
    grbStepper.enableOutputs();

    grbStepper.setCurrentPosition(0);
    
    pinMode(SC_DIR, OUTPUT);
    pinMode(SC_STEP, OUTPUT);

    sucDriver.begin();
    sucDriver.rms_current(900);
    sucDriver.microsteps(1);
    sucDriver.pwm_autoscale(true);

    sucStepper.setEnablePin(SC_EN);
    sucStepper.setPinsInverted(false, false, true);
    sucStepper.setMaxSpeed(3500);
    sucStepper.setAcceleration(5000);
    sucStepper.enableOutputs();

    sucStepper.setCurrentPosition(0);

    Timer1.initialize(100);
    Timer1.attachInterrupt([]() {
        grbStepper.run();
        sucStepper.run();
    });
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
        1000,
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
            warn("Retracting...");
            delay(1000);
            setServoAngle(GRB_ARM_L_PIN, GRB_ARM_RET_ANGLE_L);
            delay(1000);
            setServoAngle(GRB_ARM_R_PIN, GRB_ARM_RET_ANGLE_R);
            warn("Retracted");
        },
        1000,
        armRetractDeps,
        1
    );

    movements[GRABBER_CATCH] = new TimedMovement(
        []() {
            setServoAngle(GRB_L_PIN, GRB_CATCH_ANGLE_L);
            setServoAngle(GRB_R_PIN, GRB_CATCH_ANGLE_R);
        },
        1000,
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

    movements[SUCTION_DEPLOY] = new TimedMovement(
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

    movements[GRABBER_BLOCK_UP] = new TriggeredMovement(
        []() {
            grbStepper.moveTo(GRB_UP_HEIGHT * STEPS_PER_MM);
        },
        [](void* pX) {
            return grbStepper.distanceToGo() == 0;
        },
        2000,
        nullptr,
        0
    );

    movements[GRABBER_BLOCK_DOWN] = new TriggeredMovement(
        []() {
            grbStepper.moveTo(0);
        },
        [](void* pX) {
            return grbStepper.distanceToGo() == 0;
        },
        2000,
        nullptr,
        0
    );

    static MovementDependency sucBlockApplyDeps[] = {
        {SUCTION_DEPLOY, ActuatorStatus::SET}  
    };

    movements[SUCTION_BLOCK_APPLY] = new TriggeredMovement(
        []() {
            distanceSensor.startContinuous();
            sucStepper.moveTo(0);
        },
        [](void* pX) {
            bool isApplying = distanceSensor.readRangeContinuousMillimeters() <= 40;
            if (isApplying) {
                distanceSensor.stopContinuous();
            }
            return isApplying || sucStepper.distanceToGo() == 0;
        },
        2000,
        sucBlockApplyDeps,
        1
    );

    movements[SUCTION_BLOCK_UP] = new TriggeredMovement(
        []() {
            sucStepper.moveTo(SC_UP_HEIGHT * STEPS_PER_MM);
        },
        [](void* pX) {
            return sucStepper.distanceToGo() == 0;
        },
        2000,
        nullptr,
        0
    );

    movements[SUCTION_BLOCK_DOWN] = new TriggeredMovement(
        []() {
            sucStepper.moveTo(0);
        },
        [](void* pX) {
            return sucStepper.distanceToGo() == 0;
        },
        2000,
        nullptr,
        0
    );

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
        SUCTION_DEPLOY,
        SUCTION_RETRACT
    };
    actuators[SUCTION] = new Actuator(suctionMoves, sizeof(suctionMoves) / sizeof(MovementName));

    static MovementName grbBlockMoves[] = {
        GRABBER_BLOCK_UP,
        GRABBER_BLOCK_DOWN
    };
    actuators[GRB_BLOCK] = new Actuator(grbBlockMoves, sizeof(grbBlockMoves) / sizeof(MovementName));

    static MovementName sucBlockMoves[] = {
        SUCTION_BLOCK_APPLY,
        SUCTION_BLOCK_UP,
        SUCTION_BLOCK_DOWN
    };
    actuators[SUC_BLOCK] = new Actuator(sucBlockMoves, sizeof(sucBlockMoves) / sizeof(MovementName));

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
        {ARM_RETRACT, ActuatorStatus::MOVING},
        {GRABBER_CATCH, ActuatorStatus::MOVING},
        {GRABBER_BLOCK_DOWN, ActuatorStatus::MOVING},
        {SUCTION_RETRACT, ActuatorStatus::MOVING},
        {SUCTION_BLOCK_UP, ActuatorStatus::SET},
    };
    actions[FOLD] = new Action(foldSteps, sizeof(foldSteps) / sizeof(MovementDependency));

    static MovementDependency approachSteps[] = {
        {ARM_DEPLOY, ActuatorStatus::MOVING},
        {MAGNET_ATTACH, ActuatorStatus::MOVING},
        {GRABBER_RELEASE, ActuatorStatus::MOVING},
        {SUCTION_DEPLOY, ActuatorStatus::MOVING},
        {GRABBER_BLOCK_DOWN, ActuatorStatus::SET},
    };
    actions[APPROACH] = new Action(approachSteps, sizeof(approachSteps) / sizeof(MovementDependency));

    static MovementDependency transportSteps[] = {
        {ARM_DEPLOY, ActuatorStatus::MOVING},
        {MAGNET_ATTACH, ActuatorStatus::MOVING},
        {GRABBER_CATCH, ActuatorStatus::MOVING},
        {SUCTION_BLOCK_APPLY, ActuatorStatus::SET},
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
        {SUCTION_BLOCK_APPLY, ActuatorStatus::SET},
        {GRABBER_CATCH, ActuatorStatus::SET},

        // Extraction
        {ARM_RETRACT, ActuatorStatus::MOVING},
        {PUMP_ENABLE, ActuatorStatus::SET},
        {SUCTION_BLOCK_UP, ActuatorStatus::MOVING},

        // Release
        {GRABBER_RELEASE, ActuatorStatus::SET},
    };
    actions[EXTRACT_STAGE] = new Action(extractStageSteps, sizeof(extractStageSteps) / sizeof(MovementDependency));

    static MovementDependency bannerSteps[] = {
        {BANNER_RELEASE, ActuatorStatus::SET},
    };
    actions[BANNER_DEPLOY] = new Action(bannerSteps, sizeof(bannerSteps) / sizeof(MovementDependency));

    static MovementDependency s1Steps[] = {
        {SUCTION_BLOCK_DOWN, ActuatorStatus::MOVING},
        {GRABBER_BLOCK_DOWN, ActuatorStatus::SET},
    };
    actions[S1] = new Action(s1Steps, sizeof(s1Steps) / sizeof(MovementDependency));

    static MovementDependency s2Steps[] = {
        {SUCTION_BLOCK_UP, ActuatorStatus::MOVING},
        {GRABBER_BLOCK_UP, ActuatorStatus::SET},
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

    for (byte i = 0; i < pActuator->moveCount; i++) {
        Movement* pActMove = movements[pActuator->moves[i]];

        // triggered movements need to be canceled, otherwise, they will stop new movement from old one
        if (pActMove->getType() == MovementType::TRIGGERED) {
            TriggeredMovement* pTimedActMove = static_cast<TriggeredMovement*>(pActMove);

            if (pTimedActMove->timeoutTimer != nullptr) {
                Timing::cancelTimer(pTimedActMove->timeoutTimer);
                pTimedActMove->timeoutTimer = nullptr;
            }

            if (pTimedActMove->checkerTask != nullptr) {
                Timing::cancelTimer(pTimedActMove->checkerTask);
                pTimedActMove->checkerTask = nullptr;
            }
        }
    }

    if (pMovement->getType() == MovementType::TIMED) {
        TimedMovement* timedMovement = static_cast<TimedMovement*>(pMovement);
        
        if (timedMovement->timer != nullptr) {
            Timing::cancelTimer(timedMovement->timer);
            timedMovement->timer = nullptr;
        }

        timedMovement->timer = Timing::in(timedMovement->duration, [](void* pActuator) {
            Actuator& actuator = *static_cast<Actuator*>(pActuator);
            
            actuator.status = ActuatorStatus::SET;
        }, pActuator);
    } else {
        TriggeredMovement* triggeredMovement = static_cast<TriggeredMovement*>(pMovement);
        
        if (triggeredMovement->timeoutTimer != nullptr) {
            Timing::cancelTimer(triggeredMovement->timeoutTimer);
            triggeredMovement->timeoutTimer = nullptr;
        }

        triggeredMovement->timeoutTimer = Timing::in(triggeredMovement->timeout, [](void* pActuator) {
            Actuator& actuator = *static_cast<Actuator*>(pActuator);
            
            actuator.status = ActuatorStatus::SET;
        }, pActuator);

        if (triggeredMovement->checkerTask != nullptr) {
            Timing::cancelTimer(triggeredMovement->checkerTask);
            triggeredMovement->checkerTask = nullptr;
        }

        triggeredMovement->checkerTask = Timing::when(triggeredMovement->checker, [](void* pActuator) {
            Actuator& actuator = *static_cast<Actuator*>(pActuator);
            
            actuator.status = ActuatorStatus::SET;
        }, pActuator, nullptr);
    }

    pActuator->status = ActuatorStatus::MOVING;
    pMovement->callback();
}