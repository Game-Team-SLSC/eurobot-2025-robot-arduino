#include <GlobalState.h>
#include <Logger.h>

DynamicState<ActionName>* GlobalState::action = createState(ActionName::FOLD);
DynamicState<byte>* GlobalState::score = createState(byte(DEFAULT_SCORE));
DynamicState<Travel>* GlobalState::travel = createState(Travel{0, 0, 0});
DynamicState<float>* GlobalState::speedFactor = createState(1.f);
DynamicState<bool>* GlobalState::remoteConnected = createState(false);
DynamicState<RunMode>* GlobalState::runMode = createState(AUTO_GAME_START);
DynamicState<bool>* GlobalState::isRightSide = createState(true);

void GlobalState::update(RemoteData& remoteData, RemoteData& emulatedData) {
    if (runMode->get() != POSITIONS) {
        for (byte i = 0; i < _BUTCOUNT; i++) {
            if (i == GAME_START_AUTO_BTN || i == STAGES_2_AUTO_BTN || i == STAGES_3_AUTO_BTN) continue;
            
            if (remoteData.buttons[i]) {
                runMode->set(POSITIONS);
                return;
            }
        }
    }

    if (runMode->hasChanged()) {
        warn("new run mode %d", runMode->get());
    }

    runMode->set(
        remoteData.buttons[GAME_START_AUTO_BTN] ? AUTO_GAME_START :
        remoteData.buttons[STAGES_2_AUTO_BTN] ? AUTO_2_STAGE :
        remoteData.buttons[STAGES_3_AUTO_BTN] ? AUTO_3_STAGE :
        runMode->get());

    score->set(remoteData.score);

    isRightSide->set(
        remoteData.buttons[RIGHT_SIDE_BTN]? true:
        remoteData.buttons[LEFT_SIDE_BTN]? false:
        isRightSide->get()
    );

    updateRaw(runMode->get() == POSITIONS? remoteData: emulatedData);

    speedFactor->set(
        action->get() == ActionName::APPROACH? APPROACH_SPEED_FACTOR * remoteData.slider / 255.f: remoteData.slider / 255.f);
}

void GlobalState::updateRaw(RemoteData& data) {
    travel->set(Travel{
        data.joystickRight.x,
        abs(data.joystickLeft.y) > abs(data.joystickRight.y) ? data.joystickLeft.y :data.joystickRight.y,
        data.joystickLeft.x
    });

    action->set(
        data.buttons[APPROACH_BTN]? ActionName::APPROACH:
        data.buttons[TRANSPORT_BTN]? ActionName::TRANSPORT:
        data.buttons[RELEASE_BTN]? ActionName::RELEASE:
        data.buttons[EXTRACT_STAGE_BTN]? ActionName::EXTRACT_STAGE:
        data.buttons[CATCH_BTN]? ActionName::CATCH_2S:
        data.buttons[STAGE_1_BTN]? ActionName::S1:
        data.buttons[STAGE_2_BTN]? ActionName::S2:
        data.buttons[FOLD_BTN]? ActionName::FOLD:
        data.buttons[RELEASE_BANNER_BTN]? ActionName::BANNER_DEPLOY:
        action->get()
    );

}