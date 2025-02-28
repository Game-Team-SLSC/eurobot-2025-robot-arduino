#include <GlobalState.h>

DynamicState<ActionName>* GlobalState::action = createState(ActionName::FOLD);
DynamicState<byte>* GlobalState::score = createState(byte(DEFAULT_SCORE));
DynamicState<Travel>* GlobalState::travel = createState(Travel{0, 0, 0});
DynamicState<float>* GlobalState::speedFactor = createState(1.f);
DynamicState<bool>* GlobalState::remoteConnected = createState(false);
DynamicState<RunMode>* GlobalState::runMode = createState(POSITIONS);
DynamicState<bool>* GlobalState::isRightSide = createState(true);

void GlobalState::updateFromController(RemoteData& remoteData) {

    score->set(remoteData.score);
    travel->set(Travel{
        remoteData.joystickLeft.x,
        abs(remoteData.joystickLeft.y) > abs(remoteData.joystickLeft.x) ? remoteData.joystickLeft.y :remoteData.joystickRight.y,
        remoteData.joystickRight.x
    });
    speedFactor->set(remoteData.slider / 255.f);
    isRightSide->set(remoteData.sw == UP);
}