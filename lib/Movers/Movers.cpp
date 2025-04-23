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

void Movers::stop() {
    lastFL = 0;
    lastFR = 0;
    lastRL = 0;
    lastRR = 0;
    leftSB.stop();
    rightSB.stop();
}

int8_t Movers::lerp(float a, float b, float t) {
    // Calculate intermediate result as float to preserve decimal precision
    float intermediate = a + t * (b - a);
    // Round to nearest integer before casting to int8_t
    int8_t result = int8_t(round(intermediate));
    return result;
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

    float speedFactor = GlobalState::speedFactor->get();

    // warn travel

    int8_t frw = travel.forward * Y_MOVE_FACTOR * speedFactor;
    int8_t lat = travel.lateral * X_MOVE_FACTOR * speedFactor;
    int8_t yaw = travel.yaw * YAW_FACTOR * speedFactor;
    
    //error("Travel infos %d, %d, %d", frw, lat, yaw);

    int16_t flUnconstrained = - frw - yaw - lat;
    int16_t frUnconstrained = - frw + yaw + lat;
    int16_t rlUnconstrained = - frw - yaw + lat;
    int16_t rrUnconstrained = - frw + yaw - lat;

    int16_t maxUnconstrained = max(max(abs(flUnconstrained), abs(frUnconstrained)), max(abs(rlUnconstrained), abs(rrUnconstrained)));

    // map them to fit in a -127/127 range
    if (maxUnconstrained > 127) {
        flUnconstrained = flUnconstrained * 127 / maxUnconstrained;
        frUnconstrained = frUnconstrained * 127 / maxUnconstrained;
        rlUnconstrained = rlUnconstrained * 127 / maxUnconstrained;
        rrUnconstrained = rrUnconstrained * 127 / maxUnconstrained;
    }

    lastFL = lerp(lastFL, flUnconstrained, MOVERS_RAMPING);
    lastFR = lerp(lastFR, frUnconstrained, MOVERS_RAMPING);
    lastRL = lerp(lastRL, rlUnconstrained, MOVERS_RAMPING);
    lastRR = lerp(lastRR, rrUnconstrained, MOVERS_RAMPING);

    // warn("Speedfactor : %d travel : %d unconFL : %d last FL : %d", speedFactor > 0.5f, travel.forward, flUnconstrained, lastFL);

    // warn("lerp : %d, %d, %d, %d" , lastFL, lastFR, lastRL, lastRR);

    leftSB.motor(MOVER_FL, lastFL);
    leftSB.motor(MOVER_RL, lastRL);
    rightSB.motor(MOVER_FR, lastFR);
    rightSB.motor(MOVER_RR, lastRR);
};