#include <ScoreDisplay.h>

TM1637Display ScoreDisplay::display(SCORE_DP_CLK, SCORE_DP_DIO);

void ScoreDisplay::setup() {
    display.setBrightness(7);
    display.showNumberDec(DEFAULT_SCORE);
}

void ScoreDisplay::update() {
    bool isOn = true;

    if (GlobalState::score->hasChanged()) {
        display.showNumberDec(GlobalState::score->get());
    }

    if (!GlobalState::remoteConnected->get()) {
        if ((millis() / BLINK_INTERVAL) % 2) {
            isOn = false;
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