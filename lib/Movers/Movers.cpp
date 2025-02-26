#include <Movers.h>

const float LATERAL_MVT_RATIO = X_MOVE_FACTOR/255;
const float FRW_MVT_RATIO = Y_MOVE_FACTOR/255;
const float YAW_FACTOR_RATIO = YAW_FACTOR/255;

L298NX2 Movers::frontDriver(FL_EN, FL_IN1, FL_IN2, FR_EN, FR_IN1, FR_IN2);
L298NX2 Movers::rearDriver(RL_EN, RL_IN1, RL_IN2, RR_EN, RR_IN1, RR_IN2);

void Movers::setup() {
    frontDriver.stop();
    rearDriver.stop();
};

void Movers::update() {
    if (!GlobalState::remoteConnected->get()) {
        frontDriver.stop();
        rearDriver.stop();
        return;
    }
    if (!GlobalState::travel->hasChanged()) return;

    Travel travel = GlobalState::travel->get();

    char frw = travel.forward * FRW_MVT_RATIO;
    char lat = travel.lateral * LATERAL_MVT_RATIO;
    char yaw = travel.yaw * YAW_FACTOR_RATIO;

    char fl = frw + lat + yaw;
    char fr = frw - lat - yaw;
    char rl = frw - lat + yaw;
    char rr = frw + lat - yaw;

    if (fl < 0) {
        frontDriver.backwardA();
    } else {
        frontDriver.forwardA();
    }

    if (fr < 0) {
        frontDriver.backwardB();
    } else {
        frontDriver.forwardB();
    }

    if (rl < 0) {
        rearDriver.backwardA();
    } else {
        rearDriver.forwardA();
    }

    if (rr < 0) {
        rearDriver.backwardB();
    } else {
        rearDriver.forwardB();
    }

    frontDriver.setSpeedA(abs(fl) * 2);
    frontDriver.setSpeedB(abs(fr) * 2);
    rearDriver.setSpeedA(abs(rl) * 2);
    rearDriver.setSpeedB(abs(rr) * 2);
};