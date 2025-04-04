#include <LCD.h>
#include <GlobalSettings.h>
#include <GlobalState.h>

DFRobot_RGBLCD1602 LCD::screen = DFRobot_RGBLCD1602(0x6B, 16, 2);

void LCD::setup() {
    screen.init();
    screen.setPWM(screen.REG_BLUE, 200);
    screen.clear();
    screen.println("Starting up...");
}

void LCD::update() {
    if (!GlobalState::remoteConnected->hasChanged()) return;

    if (!GlobalState::remoteConnected->get()) {
        display("Signal lost...");
    } else {
        display("Robot ready !");
    }
}

void LCD::display(const char* str) {
    screen.clear();
    screen.setCursor(0, 0);
    screen.print(str);
}