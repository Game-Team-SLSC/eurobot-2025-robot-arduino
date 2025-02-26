#include <Actuators.h>
#include <GlobalState.h>
#include <ActionName.h>

Adafruit_PWMServoDriver Actuators::pwmDriver = Adafruit_PWMServoDriver(0x40);

TMC2209Stepper Actuators::grbDriver = TMC2209Stepper(&GRB_SERIAL, R_SENSE, 0b00);
TMC2209Stepper Actuators::sucDriver = TMC2209Stepper(&SC_SERIAL, R_SENSE, 0b00);

AccelStepper Actuators::grbStepper = AccelStepper(AccelStepper::DRIVER, GRB_STEP, GRB_DIR);
AccelStepper Actuators::sucStepper = AccelStepper(AccelStepper::DRIVER, SC_STEP, SC_DIR);

Movement* Actuators::movements[_MOVCOUNT];
Action* Actuators::actions[_ACTCOUNT];

void Actuators::setup() {
    pwmDriver.begin();
    pwmDriver.setOscillatorFrequency(25e6);
    pwmDriver.setPWMFreq(50);

    pinMode(GRB_DIR, OUTPUT);
    pinMode(GRB_STEP, OUTPUT);

    grbDriver.begin();
    grbDriver.rms_current(900);
    grbDriver.pwm_autoscale(true);
    grbDriver.microsteps(16);
    grbDriver.en_spreadCycle();

    grbStepper.setMaxSpeed(100 * STEPS_PER_MM);
    grbStepper.setAcceleration(75 * STEPS_PER_MM);
    grbStepper.enableOutputs();

    grbStepper.setCurrentPosition(0);
    
    pinMode(SC_DIR, OUTPUT);
    pinMode(SC_STEP, OUTPUT);

    sucDriver.begin();
    sucDriver.rms_current(900);
    sucDriver.pwm_autoscale(true);
    sucDriver.microsteps(16);
    sucDriver.en_spreadCycle();

    sucStepper.setMaxSpeed(100 * STEPS_PER_MM);
    sucStepper.setAcceleration(75 * STEPS_PER_MM);
    sucStepper.enableOutputs();

    sucStepper.setCurrentPosition(0);

    movements[MAGNET_ATTACH] = new TimedMovement(attachMagnet, 1000);
    movements[MAGNET_ATTACH]->addDep(new MovementDependency{
        MovementCall{movements[ARM_DEPLOY], true},
        true
    });
    movements[MAGNET_ATTACH]->addDep(new MovementDependency{
        MovementCall{movements[ARM_DEPLOY], true},
        false
    });

    movements[ARM_DEPLOY] = new TimedMovement(deployArm, 1000);
    movements[ARM_DEPLOY]->addDep(new MovementDependency{
        MovementCall{movements[MAGNET_ATTACH], true},
        true
    });

    
    movements[GRABBER_CATCH] = new TimedMovement(catchBlock, 1000);
    
    movements[SUCTION_DEPLOY] = new TimedMovement(deploySuction, 1000);
    movements[SUCTION_DEPLOY]->addDep(new MovementDependency{
        MovementCall{movements[PUMP_ENABLE], false},
        false
    });

    movements[GRABBER_BLOCK_UP] = new TriggeredMovement(moveGrabber, isGrabberBlockMoved);
    movements[SUCTION_BLOCK_APPLY] = new TriggeredMovement(moveSuction, isSuctionBlockMoved);
    movements[SUCTION_BLOCK_APPLY]->addDep(new MovementDependency{
        MovementCall{movements[SUCTION_DEPLOY], true},
        false
    });

    movements[PUMP_ENABLE] = new TimedMovement(enablePump, 1000);
    movements[BANNER_RELEASE] = new TimedMovement(releaseBanner, 1000);

    static MovementCall foldedMovements[] = {
        // prevent from breaking magnet
        {movements[ARM_DEPLOY], true},
        {movements[MAGNET_ATTACH], true},
        {movements[ARM_DEPLOY], false},
        
        // put stage 1
        {movements[GRABBER_BLOCK_UP], false},
        {movements[SUCTION_BLOCK_APPLY], false},

        // misc
        {movements[GRABBER_CATCH], true},
        {movements[SUCTION_DEPLOY], true},
        {movements[PUMP_ENABLE], false}
    };

    static MovementCall approachMovements[] = {
        // prevent from breaking magnet
        {movements[ARM_DEPLOY], true},
        {movements[MAGNET_ATTACH], false},
        
        // put stage 1
        {movements[GRABBER_BLOCK_UP], false},
        {movements[SUCTION_BLOCK_APPLY], false},

        // misc
        {movements[GRABBER_CATCH], false},
        {movements[SUCTION_DEPLOY], false},
        {movements[PUMP_ENABLE], false}
    };

    static MovementCall transportMovements[] = {
        // prevent from breaking magnet
        {movements[ARM_DEPLOY], true},
        {movements[MAGNET_ATTACH], true},
        
        // put stage 1
        {movements[GRABBER_BLOCK_UP], false},
        {movements[SUCTION_BLOCK_APPLY], true},

        // misc
        {movements[GRABBER_CATCH], true},
        {movements[SUCTION_DEPLOY], true},
        {movements[PUMP_ENABLE], false}
    };

    static MovementCall releaseMovements[] = {
        // prevent from breaking magnet
        {movements[ARM_DEPLOY], true},
        {movements[MAGNET_ATTACH], false},
        
        // can be any stage
        //{movements[GRABBER_BLOCK_UP], false},
        //{movements[SUCTION_BLOCK_APPLY], false},

        // misc
        {movements[GRABBER_CATCH], false},
        {movements[SUCTION_DEPLOY], false},
        {movements[PUMP_ENABLE], false}
    };

    static MovementCall lateralMovements[] = {
        // prevent from breaking magnet
        {movements[ARM_DEPLOY], true},
        {movements[MAGNET_ATTACH], false},
        {movements[ARM_DEPLOY], false},
        
        // can be any stage
        // {movements[GRABBER_BLOCK_UP], false},
        // {movements[SUCTION_BLOCK_APPLY], false},

        // misc
        {movements[GRABBER_CATCH], false},
        {movements[SUCTION_DEPLOY], false},
        {movements[PUMP_ENABLE], false}
    };

    static MovementCall catchMovements[] = {
        // prevent from breaking magnet
        {movements[ARM_DEPLOY], true},
        {movements[MAGNET_ATTACH], false},
        {movements[ARM_DEPLOY], false},
        
        // can be any stage
        // {movements[GRABBER_BLOCK_UP], false},
        // {movements[SUCTION_BLOCK_APPLY], false},

        // misc
        {movements[GRABBER_CATCH], true},
        {movements[SUCTION_DEPLOY], false},
        {movements[PUMP_ENABLE], false}
    };

    static MovementCall stage1[] = {
        // prevent from breaking magnet
        // {movements[ARM_DEPLOY], true},
        // {movements[MAGNET_ATTACH], false},
        // {movements[ARM_DEPLOY], false},
        
        // can be any stage
        {movements[GRABBER_BLOCK_UP], false},
        {movements[SUCTION_BLOCK_APPLY], false},

        // misc
        // {movements[GRABBER_CATCH], true},
        // {movements[SUCTION_DEPLOY], false},
        // {movements[PUMP_ENABLE], false}
    };

    static MovementCall stage2[] = {
        // prevent from breaking magnet
        // {movements[ARM_DEPLOY], true},
        // {movements[MAGNET_ATTACH], false},
        // {movements[ARM_DEPLOY], false},
        
        // can be any stage
        {movements[GRABBER_BLOCK_UP], true},
        {movements[SUCTION_BLOCK_APPLY], true},

        // misc
        // {movements[GRABBER_CATCH], true},
        // {movements[SUCTION_DEPLOY], false},
        // {movements[PUMP_ENABLE], false}
    };

    static MovementCall bannerMovements[] = {
        {movements[BANNER_RELEASE], true}
    };

    actions[FOLDED] = new Action(foldedMovements, sizeof(foldedMovements) / sizeof(MovementCall));
    actions[APPROACH] = new Action(approachMovements, sizeof(approachMovements) / sizeof(MovementCall));
    actions[TRANSPORT] = new Action(transportMovements, sizeof(transportMovements) / sizeof(MovementCall));
    actions[RELEASE] = new Action(releaseMovements, sizeof(releaseMovements) / sizeof(MovementCall));
    actions[LATERAL] = new Action(lateralMovements, sizeof(lateralMovements) / sizeof(MovementCall));
    actions[CATCH] = new Action(catchMovements, sizeof(catchMovements) / sizeof(MovementCall));
    actions[S1] = new Action(stage1, sizeof(stage1) / sizeof(MovementCall));
    actions[S2] = new Action(stage2, sizeof(stage2) / sizeof(MovementCall));
    actions[BANNER] = new Action(bannerMovements, sizeof(bannerMovements) / sizeof(MovementCall));
}

void Actuators::update()  {
    Action action = *actions[GlobalState::action->get()];
    if (GlobalState::action->hasChanged()) {
        action.execute();
        return;
    }

    action.update();
}

// @param angle The desired angle in degrees
void Actuators::setServoAngle(byte num, byte angle) {
    pwmDriver.writeMicroseconds(num, map(angle, 0, 180, 1000, 2000));
}

void Actuators::enablePump(bool enable) {
    digitalWrite(PUMP_ENABLE, enable);
}

void Actuators::attachMagnet(bool attach) {
    setServoAngle(GRB_MAGNET_L_PIN, attach ? GRB_MAGNET_ATTACH_ANGLE_L : GRB_MAGNET_RELEASE_ANGLE_L);
    setServoAngle(GRB_MAGNET_R_PIN, attach ? GRB_MAGNET_ATTACH_ANGLE_R : GRB_MAGNET_RELEASE_ANGLE_R);
}

void Actuators::deployArm(bool deploy) {
    setServoAngle(GRB_ARM_L_PIN, deploy ? GRB_ARM_DEP_ANGLE_L : GRB_ARM_RET_ANGLE_L);
    setServoAngle(GRB_ARM_R_PIN, deploy ? GRB_ARM_DEP_ANGLE_R : GRB_ARM_RET_ANGLE_R);
}

void Actuators::catchBlock(bool _catch) {
    setServoAngle(GRB_L_PIN, _catch ? GRB_CATCH_ANGLE_L : GRB_RELEASE_ANGLE_L);
    setServoAngle(GRB_R_PIN, _catch ? GRB_CATCH_ANGLE_R : GRB_RELEASE_ANGLE_R);
}

void Actuators::deploySuction(bool deploy) {
    setServoAngle(SC_L_PIN, deploy ? SC_DEP_ANGLE_L : SC_RET_ANGLE_L);
    setServoAngle(SC_R_PIN, deploy ? SC_DEP_ANGLE_R : SC_RET_ANGLE_R);
}

void Actuators::moveGrabber(bool up) {
    // TODO
}

void Actuators::moveSuction(bool up) {
    // TODO
}

void Actuators::releaseBanner(bool release) {
    setServoAngle(BANNER_PIN, release ? BANNER_DEP_ANGLE : BANNER_RET_ANGLE);
}

bool Actuators::isGrabberBlockMoved(bool up) {
    // TODO
    return true;
}

bool Actuators::isSuctionBlockMoved(bool up) {
    // TODO
    return true;
}