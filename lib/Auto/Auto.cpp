#include <Auto.h>
#include <Logger.h>
#include <GlobalState.h>
#include <GlobalSettings.h>
#include <Actuators.h>

AutoStep* Auto::stepsBuffer[MAX_STEPS];
byte Auto::stepCount = 0;
RemoteData* Auto::emulatedData = nullptr;
bool Auto::isRunning = true;

byte Auto::lastRunMode = POSITIONS; // cuz the run mode state is resetting at the end of the main loop

void Auto::setup() {
    resetData();
}

void Auto::clearBuffer() {
    for (byte i = 0; i < MAX_STEPS; i++) {
        stepsBuffer[i] = nullptr;
    };

    stepCount = 0;
}

void Auto::resetData() {
    setJoysticks(0, 0, 0);
    for (byte i = 0; i < _BUTCOUNT; i++) {
        emulatedData->buttons[i] = false;
    }
    emulatedData->slider = 0;
    emulatedData->score = DEFAULT_SCORE;
}

void Auto::pressButton(Buttons button) {
    Buttons* heapButton = new Buttons(button);

    emulatedData->buttons[button] = true;
    Timing::in(10, [](void* pBtn) {
        Buttons* pCastedBtn = static_cast<Buttons*>(pBtn);
        emulatedData->buttons[*pCastedBtn] = false;
        delete pCastedBtn;
    }, heapButton, true);
}

void Auto::setJoysticks(int8_t x, int8_t y, int8_t z) {
    emulatedData->joystickLeft.x = 128 + x;
    emulatedData->joystickLeft.y = 128 + y;
    emulatedData->joystickRight.x = 128 + z;
}

void Auto::exec2Stages() {
    stepsBuffer[stepCount++] = new TriggeredAutoStep([]() {
        pressButton(EXTRACT_STAGE_BTN);
    }, []() {
        return GlobalState::action->get() == ActionName::EXTRACT_STAGE && Actuators::isActionRunning() == false;
    });

    stepsBuffer[stepCount++] = new TimedAutoStep([]() {
        setJoysticks(0, 30, 0);
    }, 2000);

    stepsBuffer[stepCount++] = new TriggeredAutoStep([]() {
        setJoysticks(0, 0, 0);
        pressButton(STAGE_2_BTN);
    }, []() {
        return GlobalState::action->get() == ActionName::S2 && Actuators::isActionRunning() == false;
    });

    stepsBuffer[stepCount++] = new TimedAutoStep([]() {
        setJoysticks(0, -30, 0);
    }, 2000);

    stepsBuffer[stepCount++] = new TriggeredAutoStep([]() {
        setJoysticks(0, 0, 0);
        pressButton(RELEASE_BTN);
    }, []() {
        return GlobalState::action->get() == ActionName::RELEASE && Actuators::isActionRunning() == false;
    });
}

void Auto::exec3Stages() {
    stepsBuffer[stepCount++] = new TriggeredAutoStep([]() {
        pressButton(CATCH_2S_BTN);
    }, []() {
        return GlobalState::action->get() == ActionName::EXTRACT_STAGE && Actuators::isActionRunning() == false;
    });

    stepsBuffer[stepCount++] = new TriggeredAutoStep([]() {
        pressButton(STAGE_2_BTN);
    }, []() {
        return GlobalState::action->get() == ActionName::S2 && Actuators::isActionRunning() == false;
    });

    stepsBuffer[stepCount++] = new TimedAutoStep([]() {
        setJoysticks(0, 30, 0);
    }, 2000);

    stepsBuffer[stepCount++] = new TriggeredAutoStep([]() {
        setJoysticks(0, 0, 0);
        pressButton(RELEASE_BTN);
    }, []() {
        return GlobalState::action->get() == ActionName::RELEASE && Actuators::isActionRunning() == false;
    });
}

void Auto::execGameStart() {
    stepsBuffer[stepCount++] = new TriggeredAutoStep([]() {
        pressButton(RELEASE_BANNER_BTN);
    }, []() {
        return GlobalState::action->get() == ActionName::BANNER_DEPLOY && Actuators::isActionRunning() == false;
    });

    stepsBuffer[stepCount++] = new TimedAutoStep([]() {
        setJoysticks(0, 100, 0);
    }, 2000);

    stepsBuffer[stepCount++] = new TimedAutoStep([]() {
        pressButton(APPROACH_BTN);
        setJoysticks(0, 0, -70);
    }, 1000);

    stepsBuffer[stepCount++] = new TimedAutoStep([]() {
        setJoysticks(GlobalState::isRightSide? -100: 100, 0, 0);
    }, 2000);

    stepsBuffer[stepCount++] = new TimedAutoStep([]() {
        setJoysticks(0, 50, 0);
    }, 3000);

    stepsBuffer[stepCount++] = new TriggeredAutoStep([]() {
        pressButton(TRANSPORT_BTN);
        setJoysticks(0, 0, 0);
    }, []() {
        return GlobalState::action->get() == ActionName::TRANSPORT && Actuators::isActionRunning() == false;
    });

    stepsBuffer[stepCount++] = new TimedAutoStep([]() {
        setJoysticks(0, 50, 0);
    }, 1000);

    stepsBuffer[stepCount++] = new TimedAutoStep([]() {
        setJoysticks(0, 0, 0);
    }, 0);
}

void Auto::fetchData(RemoteData& dataBuffer) {
    emulatedData = &dataBuffer;

    if (lastRunMode != GlobalState::runMode->get()) {
        lastRunMode = GlobalState::runMode->get();
        resetData();
        execMode();
    }
    

    if (stepCount == 0) {
        if (isRunning) {
            isRunning = false;
            info("Automode %d finished !", GlobalState::runMode->get());

            if (GlobalState::runMode->get() == AUTO_GAME_START) {
                GlobalState::runMode->set(AUTO_2_STAGE);
                resetData();
                execMode();
            } else GlobalState::runMode->set(POSITIONS);
        }
        return;
    }

    if (!stepsBuffer[0]->wasCalled) {
        execStep();
    }

    if (stepsBuffer[0]->getType() == TIMED_STEP) {
        TimedAutoStep* pStep = static_cast<TimedAutoStep*>(stepsBuffer[0]);

        if (millis() - pStep->startTime >= pStep->duration) {
            stepsBuffer[0]->isFinished = true;
        }
    } else if (stepsBuffer[0]->getType() == TRIGGERED_STEP) {
        TriggeredAutoStep* pStep = static_cast<TriggeredAutoStep*>(stepsBuffer[0]);

        if (pStep->checker()) {
            stepsBuffer[0]->isFinished = true;
        }
    }
    
    if (stepsBuffer[0]->isFinished) {
        delete stepsBuffer[0];

        info("Autostep finished");

        for (byte i = 0; i < stepCount - 1; i++) {
            stepsBuffer[i] = stepsBuffer[i + 1];
        }

        stepsBuffer[stepCount - 1] = nullptr;
        stepCount--;
    }
}

void Auto::execMode() {
    info("Auto mode %d started", GlobalState::runMode->get());
    isRunning = true;
    clearBuffer();
    switch (GlobalState::runMode->get()) {
        case AUTO_GAME_START:
            execGameStart();
            break;
        case AUTO_2_STAGE:
            exec2Stages();
            break;
        case AUTO_3_STAGE:
            exec3Stages();
            break;
        default:
            break;
    }
}

void Auto::execStep() {
    info("Calling autostep. %d autosteps remaining", stepCount);
    stepsBuffer[0]->callback();
    stepsBuffer[0]->wasCalled = true;
    stepsBuffer[0]->isFinished = false;

    if (stepsBuffer[0]->getType() == AutoStepType::TIMED_STEP) {
        TimedAutoStep* pStep = static_cast<TimedAutoStep*>(stepsBuffer[0]);

        pStep->startTime = millis();
    }
}