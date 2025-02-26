#include <Auto.h>
#include <GlobalState.h>
#include <GlobalSettings.h>
#include <Timing.h>
#include <Actuators.h>

RemoteData Auto::emulatedData = {
    .joystickLeft = {
        .x = 0,
        .y = 0,
        .sw = false
    },
    .joystickRight = {
        .x = 0,
        .y = 0,
        .sw = false
    },
    .buttons = {false, false, false, false, false, false, false, false, false, false},
    .slider = 0,
    .score = 0,
    .sw = MIDDLE
};



void Auto::fetchData(RemoteData& dataBuffer) {
    switch (GlobalState::runMode->hasChanged()) {
        case AUTO_GAME_START:
            runGameStart();
            break;
        case AUTO_STACK_2:
            // TODO
            break;
        case AUTO_STACK_3:
            // TODO
            break;
    }
}

void Auto::pressButton(byte button) {
    emulatedData.buttons[button] = true;
    Timing::in(0, [](void* btn) {
        emulatedData.buttons[static_cast<byte>(*static_cast<byte*>(btn))] = false;
    }, &button, true);
}

void Auto::runGameStart() {
    pressButton(RELEASE_BANNER_BTN);

    // wait till banner is released
    if (Actuators::movements[BANNER_RELEASE]->getMovementStatus() != SET);

    emulatedData.joystickLeft = {
        .x = 128 + 64,
        .y = 0,
        .sw = false
    };

    // wait for some seconds for forward

    emulatedData.joystickLeft = {
        .x = 0,
        .y = 128 + 64,
        .sw = false
    };

    // wait for some seconds for 180 degrees turn

    emulatedData.joystickLeft = {
        .x = 0,
        .y = 128 + 64,
        .sw = false
    };

    // wait for some seconds for lateral

    emulatedData.joystickRight = {
        .x = 0,
        .y = 128,
        .sw = false
    };

    // wait for some seconds for forward

    emulatedData.joystickLeft = {
        .x = 0,
        .y = 128,
        .sw = false
    };

    // wait for some seconds for 90 degrees turn

    emulatedData.joystickLeft = {
        .x = 0,
        .y = 0,
        .sw = false
    };

    

    emulatedData.joystickLeft
}