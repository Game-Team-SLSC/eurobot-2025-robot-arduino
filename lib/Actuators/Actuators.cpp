#include "Actuators.h"
#include <GlobalSettings.h>
#include <GlobalState.h>
#include <Movers.h>
#include <Wire.h>
#include <PulleyPosition.h>

Adafruit_PWMServoDriver Actuators::pwmDriver = Adafruit_PWMServoDriver(0x40);
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
    for (byte i = 0; i < stepsCount; i++) {
        stepsBuffer[i] = nullptr;
    }
    stepsCount = 0;

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

    ESP_SERIAL.begin(ESP_BAUD);

    pinMode(PUMP_RLY, OUTPUT);
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
        1
    );

    movements[GRABBER_CATCH] = new TimedMovement(
        []() {
            setServoAngle(GRB_L_PIN, GRB_CATCH_ANGLE_L);
            setServoAngle(GRB_R_PIN, GRB_CATCH_ANGLE_R);
        },
        75,
        nullptr,
        0
    );

    movements[GRABBER_RELEASE] = new TimedMovement(
        []() {
            setServoAngle(GRB_L_PIN, GRB_RELEASE_ANGLE_L);
            setServoAngle(GRB_R_PIN, GRB_RELEASE_ANGLE_R);
        },
        500,
        nullptr,
        0
    );

    movements[SUCTION_APPLY] = new TimedMovement(
        []() {
            setServoAngle(SC_L_PIN, SC_DEP_ANGLE_L, SF20);
            setServoAngle(SC_R_PIN, SC_DEP_ANGLE_R, SF20);
        },
        700,
        nullptr,
        0
    );

    movements[SUCTION_LIFT] = new TimedMovement(
        []() {
            setServoAngle(SC_L_PIN, SC_LIFT_ANGLE_L, SF20);
            setServoAngle(SC_R_PIN, SC_LIFT_ANGLE_R, SF20);
        },
        200,
        nullptr,
        0
    );

    static MovementDependency sucRetractDeps[] = {
        {PUMP_DISABLE, ActuatorStatus::SET}
    };
    movements[SUCTION_RETRACT] = new TimedMovement(
        []() {
            setServoAngle(SC_L_PIN, SC_RET_ANGLE_L, SF20);
            setServoAngle(SC_R_PIN, SC_RET_ANGLE_R, SF20);
        },
        1000,
        sucRetractDeps,
        1
    );
    movements[PLATFORM_UP] = new TriggeredMovement(
        []() {
            ESP_SERIAL.write(PulleyPosition::UP_POS);
        },
        []() {
            return (ESP_SERIAL.available() && ESP_SERIAL.read() == PulleyPosition::UP_POS);
        },
        2000,
        nullptr,
        0
    );

    movements[PLATFORM_TRANS] = new TriggeredMovement(
        []() {
            ESP_SERIAL.write(PulleyPosition::TRANS_POS);
        },
        []() {
            return (ESP_SERIAL.available() && ESP_SERIAL.read() == PulleyPosition::TRANS_POS);
        },
        2000,
        nullptr,
        0
    );

    movements[PLATFORM_DOWN] = new TriggeredMovement(
        []() {
            ESP_SERIAL.write(PulleyPosition::DOWN_POS);
        },
        []() {
            return (ESP_SERIAL.available() && ESP_SERIAL.read() == PulleyPosition::DOWN_POS);
        },
        2000,
        nullptr,
        0
    );

    movements[BANNER_RELEASE] = new TimedMovement(
        []() {
            setServoAngle(BANNER_PIN, BANNER_DEP_ANGLE);
        },
        1000,
        nullptr,
        0
    );

    movements[BANNER_CATCH] = new TimedMovement(
        []() {
            setServoAngle(BANNER_PIN, BANNER_RET_ANGLE);
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
        600,
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
        PLATFORM_TRANS,
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
        {PUMP_DISABLE, ActuatorStatus::MOVING},
        {BANNER_CATCH, ActuatorStatus::MOVING},
        {PLATFORM_DOWN, ActuatorStatus::SET},
        {ARM_RETRACT, ActuatorStatus::MOVING},
        {GRABBER_CATCH, ActuatorStatus::MOVING},
        {SUCTION_APPLY, ActuatorStatus::MOVING},
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
        {BANNER_CATCH, ActuatorStatus::MOVING},
        {ARM_DEPLOY, ActuatorStatus::MOVING},
        {MAGNET_ATTACH, ActuatorStatus::MOVING},
        {GRABBER_CATCH, ActuatorStatus::MOVING},
        {SUCTION_APPLY, ActuatorStatus::SET},
        {PLATFORM_TRANS, ActuatorStatus::SET}
    };
    actions[TRANSPORT] = new Action(transportSteps, sizeof(transportSteps) / sizeof(MovementDependency));
    
    static MovementDependency catchSteps[] = {
        {ARM_RETRACT, ActuatorStatus::MOVING},
        {GRABBER_CATCH, ActuatorStatus::SET}
    };
    actions[CATCH] = new Action(catchSteps, sizeof(catchSteps) / sizeof(MovementDependency));
    
    static MovementDependency extractStageSteps[] = {
        // Secure catch
        {PLATFORM_DOWN, ActuatorStatus::SET},
        {PUMP_ENABLE, ActuatorStatus::MOVING},
        {GRABBER_CATCH, ActuatorStatus::MOVING},
        {SUCTION_APPLY, ActuatorStatus::SET},
        
        // Extraction
        {ARM_RETRACT, ActuatorStatus::MOVING},
        {SUCTION_LIFT, ActuatorStatus::MOVING},

        // Release
        {GRABBER_RELEASE, ActuatorStatus::SET},
    };
    actions[EXTRACT_STAGE] = new Action(extractStageSteps, sizeof(extractStageSteps) / sizeof(MovementDependency));

    static MovementDependency preReleaseSteps[] = {
        {ARM_DEPLOY, ActuatorStatus::MOVING},
        {SUCTION_APPLY, ActuatorStatus::MOVING},
    };
    actions[PRERELEASE_STAGE] = new Action(preReleaseSteps, sizeof(preReleaseSteps) / sizeof(MovementDependency));

    static MovementDependency releaseStageSteps[] = {
        {MAGNET_DETACH, ActuatorStatus::MOVING},
        {PUMP_DISABLE, ActuatorStatus::MOVING},
        {SUCTION_APPLY, ActuatorStatus::SET},
        {SUCTION_RETRACT, ActuatorStatus::MOVING},
        {GRABBER_RELEASE, ActuatorStatus::SET}
    };
    actions[RELEASE_STAGE] = new Action(releaseStageSteps, sizeof(releaseStageSteps) / sizeof(MovementDependency));
    
    static MovementDependency releaseUpStageSteps[] = {
        {MAGNET_DETACH, ActuatorStatus::MOVING},
        {PUMP_DISABLE, ActuatorStatus::SET},
        {SUCTION_APPLY, ActuatorStatus::SET},
        {SUCTION_RETRACT, ActuatorStatus::MOVING},
        {GRABBER_RELEASE, ActuatorStatus::SET}
    };
    actions[RELEASE_UP_STAGE] = new Action(releaseUpStageSteps, sizeof(releaseUpStageSteps) / sizeof(MovementDependency));

    static MovementDependency releaseGrabberSteps[] = {
        {GRABBER_RELEASE, ActuatorStatus::SET}
    };
    actions[RELEASE_GRABBER] = new Action(releaseGrabberSteps, sizeof(releaseGrabberSteps) / sizeof(MovementDependency));


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
}

void Actuators::setServoAngle(byte pin, short angle, ServoType servoType) {
    pwmDriver.writeMicroseconds(pin, map(angle, 0, servoType == MG996R? 180: 270, 400, servoType == MG996R? 2850: 2500));
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