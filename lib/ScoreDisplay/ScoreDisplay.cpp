#include <ScoreDisplay.h>

TM1637Display ScoreDisplay::display(SCORE_DP_CLK, SCORE_DP_DIO);
bool ScoreDisplay::isOn = true;
EveryTimer* ScoreDisplay::timer = nullptr;

void ScoreDisplay::setup() {
    display.setBrightness(7);
    display.showNumberDec(DEFAULT_SCORE);
}

void ScoreDisplay::update() {
    if (GlobalState::score->hasChanged()) {
        info("Update score display : %d", GlobalState::score->get());
        display.showNumberDec(GlobalState::score->get());
    }

    if (GlobalState::remoteConnected->hasChanged()) {
        if (!GlobalState::remoteConnected->get()) {

            isOn = false;
            info("Blinking score display");
            timer = Timing::every(
                BLINK_INTERVAL,
                +[](void* _) -> bool{
                    ScoreDisplay::isOn = !ScoreDisplay::isOn;
                    return true;
                });
        } else if (timer != nullptr) {
            isOn = true;

            info("Stopping blinking score display");
            Timing::cancelTimer(timer);
            timer = nullptr;
        }
    }

    if (isOn) {
        display.setBrightness(7, true);
        display.showNumberDec(GlobalState::score->get());
    } else {
        display.setBrightness(7, false);
        display.showNumberDec(GlobalState::score->get());
    }
}