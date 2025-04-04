#include <Movers.h>

const Travel stopTravel = {0, 0, 0};

SoftwareSerial Movers::leftSerial(NOT_A_PIN, SBR_L);
SoftwareSerial Movers::rightSerial(NOT_A_PIN, SBR_R);
SabertoothSimplified Movers::leftSB(leftSerial);
SabertoothSimplified Movers::rightSB(rightSerial);

int8_t Movers::lastFR = 0;
int8_t Movers::lastFL = 0;
int8_t Movers::lastRR = 0;
int8_t Movers::lastRL = 0;

void Movers::setup() {
    leftSerial.begin(9600);
    rightSerial.begin(9600);
    leftSB.stop();
    rightSB.stop();
};

int8_t Movers::lerp(int8_t a, int8_t b, float t) {
    return a + int8_t(t * (b - a));
}

void Movers::update() {
    if (!GlobalState::remoteConnected->get()) {
        if (!GlobalState::remoteConnected->hasChanged()) return;
        info("Stopping movers");
        leftSB.stop();
        rightSB.stop();
        return;
    }
    
    Travel travel = GlobalState::travel->get();

    if (travel == stopTravel) {
        info("Stopping movers");
        leftSB.stop();
        rightSB.stop();
        return;
    }

    float speedFactor = GlobalState::speedFactor->get();

    int8_t frw = travel.forward * Y_MOVE_FACTOR * speedFactor;
    int8_t lat = travel.lateral * X_MOVE_FACTOR * speedFactor;
    int8_t yaw = travel.yaw * YAW_FACTOR * speedFactor;

    int16_t flUnconstrained = - frw - yaw - lat;
    int16_t frUnconstrained = - frw + yaw + lat;
    int16_t rlUnconstrained = frw + yaw - lat;
    int16_t rrUnconstrained = frw - yaw + lat;

    lastFL = constrain(lerp(lastFL, flUnconstrained, MOVERS_RAMPING), -127, 127);
    lastFR = constrain(lerp(lastFR, frUnconstrained, MOVERS_RAMPING), -127, 127);
    lastRL = constrain(lerp(lastRL, rlUnconstrained, MOVERS_RAMPING), -127, 127);
    lastRR = constrain(lerp(lastRR, rrUnconstrained, MOVERS_RAMPING), -127, 127);

    leftSB.motor(MOVER_FL, lastFL);
    leftSB.motor(MOVER_RL, lastRL);
    rightSB.motor(MOVER_FR, lastFR);
    rightSB.motor(MOVER_RR, lastRR);
};