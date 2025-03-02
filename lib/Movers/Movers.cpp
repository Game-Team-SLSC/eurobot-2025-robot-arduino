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
        if (!GlobalState::remoteConnected->hasChanged()) return;
        info("Stopping movers");
        frontDriver.stop();
        rearDriver.stop();
        return;
    }
    if (!GlobalState::travel->hasChanged() && !GlobalState::speedFactor->hasChanged()) return;

    Travel travel = GlobalState::travel->get();

    float speedFactor = GlobalState::speedFactor->get();

    char speedFactorStr[10];
    dtostrf(speedFactor, 4, 2, speedFactorStr);
    info("Speed factor : %s", speedFactorStr);

    int8_t frw = travel.forward * FRW_MVT_RATIO * speedFactor;
    int8_t lat = travel.lateral * LATERAL_MVT_RATIO * speedFactor;
    int8_t yaw = travel.yaw * YAW_FACTOR_RATIO * speedFactor;

    int8_t fl = frw + lat + yaw;
    int8_t fr = frw - lat - yaw;
    int8_t rl = frw - lat + yaw;
    int8_t rr = frw + lat - yaw;

    info("Set motors\n - FL: %d\n - FR: %d\n - RL: %d\n - RR: %d", fl, fr, rl, rr);

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