#include <GlobalState.h>
#include <Logger.h>

DynamicState<ActionName>* GlobalState::action = createState(ActionName::FOLD);
DynamicState<byte>* GlobalState::score = createState(byte(DEFAULT_SCORE));
DynamicState<Travel>* GlobalState::travel = createState(Travel{0, 0, 0});
DynamicState<float>* GlobalState::speedFactor = createState(1.f);
DynamicState<bool>* GlobalState::remoteConnected = createState(false);
DynamicState<RunMode>* GlobalState::runMode = createState(MANUAL);

void GlobalState::update(RemoteData& remoteData, RemoteData& emulatedData) {
    if (runMode->get() != MANUAL) {
        for (byte i = 0; i < _BUTCOUNT; i++) {
            if (i == STAGES_2_AUTO_BTN || i == STAGES_3_AUTO_BTN) continue;
            
            if (remoteData.buttons[i]) {
                warn("Back to positions");
                runMode->set(MANUAL);
                return;
            }
        }
    }

    runMode->set(
        remoteData.buttons[STAGES_2_AUTO_BTN] ? AUTO_2_STAGE :
        remoteData.buttons[STAGES_3_AUTO_BTN] ? AUTO_3_STAGE :
        remoteData.buttons[RELEASE_3S_AUTO_BTN] ? AUTO_RELEASE_3_STAGE :
        runMode->get());

    if (runMode->hasChanged()) {
        info("new run mode %d", runMode->get());
    }

    score->set(remoteData.score);

    updateRaw(runMode->get() == MANUAL? remoteData: emulatedData);

    if (runMode->get() != RunMode::MANUAL) {
        speedFactor->set(1.f);
    } else if (action->get() == ActionName::APPROACH) {
        speedFactor->set(APPROACH_SPEED_FACTOR * remoteData.slider / 255.f);
    } else if (action->get() == ActionName::S2) {
        speedFactor->set(S2_SPEED_FACTOR * remoteData.slider / 255.f);
    } else if (action->get() == ActionName::TRANSPORT) {
        speedFactor->set(TRANSPORT_SPEED_FACTOR * remoteData.slider / 255.f);
    } else {
        speedFactor->set(remoteData.slider / 255.f);
    }
}

void GlobalState::updateRaw(RemoteData& data) {
    int8_t forward = map(255 - data.joystickLeft.y, 0, 256, -127, 128);
    int8_t lateral = map(data.joystickLeft.x, 0, 256, -127, 128);
    int8_t yaw = map(data.joystickRight.x, 0, 256, -127, 128);

    if (abs(forward) < DEADZONE) forward = 0;
    if (abs(lateral) < DEADZONE) lateral = 0;
    if (abs(yaw) < DEADZONE) yaw = 0;
    
    travel->set(Travel{forward, lateral, yaw});

    //warn("Travel infos %d, %d, %d", travel->get().forward, travel->get().lateral, travel->get().yaw);

    action->set(
        data.buttons[FOLD_BTN]? ActionName::FOLD:

        data.buttons[APPROACH_BTN]? ActionName::APPROACH:
        data.buttons[TRANSPORT_BTN]? ActionName::TRANSPORT:
        data.buttons[CATCH_BTN]? ActionName::CATCH:

        data.buttons[EXTRACT_STAGE_BTN]? ActionName::EXTRACT_STAGE:
        data.buttons[PRE_RELEASE_BTN] ? ActionName::PRERELEASE_STAGE:
        data.buttons[RELEASE_STAGE_BTN]? ActionName::RELEASE_STAGE:
        data.buttons[RELEASE_UP_STAGE_BTN]? ActionName::RELEASE_UP_STAGE:
        data.buttons[RELEASE_GRABBER_BTN]? ActionName::RELEASE_GRABBER:
        
        data.buttons[STAGE_1_BTN]? ActionName::S1:
        data.buttons[STAGE_2_BTN]? ActionName::S2:

        data.buttons[RELEASE_BANNER_BTN]? ActionName::BANNER_DEPLOY:
        
        action->get()
    );

}