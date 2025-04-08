#pragma once

// WARNING : A TOO LOW VALUE MAY BLOCK PROGRAM TRANSFER (AVRDUDE: stk500_getsync() attempt 10 of 10: not in sync: resp=0x00)
// values between 50 and 200 are recommended. Increasing the value will diminish the responsiveness of the robot.

#define LOOP_DELAY 5

// DEBUG

//#define MOCK_REMOTE

#define LOG_STATE

#ifdef LOG_STATE

#define ERROR_STATE
#define WARN_STATE
#define INFO_STATE

#endif

#define INFO_TAG "\033[34m[INFO] - "
#define WARN_TAG "\033[33m[WARN] - "
#define ERROR_TAG "\033[31m[ERROR] - "

// LCD

#define LCD_I2C_ADDR 0x6B

// RF24

#define RF_CE 11
#define RF_CS 12
#define RF_MISO 50
#define RF_MOSI 51
#define RF_SCK 52
#define RF_IRQ 2
#define RF_TIMEOUT 1000
#define RF_ADDRESS "CR912"
#define RF_RETRY_DELAY 1000

// STEPPERS

#define PLATFROM_HEIGHT 120 //mm

#define PT_DIR_L 26
#define PT_STEP_L 27
#define PT_EN_L 28
#define PT_SERIAL_L Serial1

#define PT_DIR_R 30
#define PT_STEP_R 31
#define PT_EN_R 32
#define PT_SERIAL_R Serial2

#define R_SENSE 0.11f
#define STEPS_PER_MM 60
#define ADDRESS 0b00

// SCORE

#define BLINK_INTERVAL 500
#define DEFAULT_SCORE 70
#define SCORE_DP_DIO A0
#define SCORE_DP_CLK A1

// MOVERS

#define MOVERS_RAMPING 0.15f
#define MOVERS_TIMEOUT 1000

#define YAW_FACTOR 1.f
#define X_MOVE_FACTOR 0.8f
#define Y_MOVE_FACTOR 1.f

#define SBR_L 46
#define SBR_R 5

#define MOVER_RL 2
#define MOVER_FL 1
#define MOVER_RR 2
#define MOVER_FR 1

#define APPROACH_SPEED_FACTOR 0.3

// ACTUATORS

#define PCA9685_ADDR 0x40

#define GRB_L_PIN 4
#define GRB_RELEASE_ANGLE_L 90
#define GRB_CATCH_ANGLE_L 105

#define GRB_R_PIN 8
#define GRB_RELEASE_ANGLE_R 88
#define GRB_CATCH_ANGLE_R 73

#define GRB_MAGNET_L_PIN 3
#define GRB_MAGNET_ATTACH_ANGLE_L 90
#define GRB_MAGNET_RELEASE_ANGLE_L 160

#define GRB_MAGNET_R_PIN 15
#define GRB_MAGNET_ATTACH_ANGLE_R 170
#define GRB_MAGNET_RELEASE_ANGLE_R 60

#define GRB_ARM_L_PIN 0
#define GRB_ARM_DEP_ANGLE_L 122
#define GRB_ARM_RET_ANGLE_L 170

#define GRB_ARM_R_PIN 12
#define GRB_ARM_DEP_ANGLE_R 55
#define GRB_ARM_RET_ANGLE_R 10

#define SC_L_PIN 30
#define SC_DEP_ANGLE_L 180
#define SC_RET_ANGLE_L 60

#define SC_R_PIN 30
#define SC_DEP_ANGLE_R 0
#define SC_RET_ANGLE_R 120

#define BANNER_PIN 30
#define BANNER_DEP_ANGLE 0
#define BANNER_RET_ANGLE 180

// BUTTONS CONFIG

enum Buttons {
    // Auto
    GAME_START_AUTO_BTN,
    STAGES_2_AUTO_BTN,
    STAGES_3_AUTO_BTN,

    // Actions
    APPROACH_BTN,
    TRANSPORT_BTN,
    RELEASE_BTN,
    EXTRACT_STAGE_BTN,

    // Secondary actions
    RELEASE_BANNER_BTN,
    CATCH_BTN,
    STAGE_2_BTN,
    STAGE_1_BTN,
    FOLD_BTN,
    SLOW_DOWN_BTN,
    
    RIGHT_SIDE_BTN,
    LEFT_SIDE_BTN,

    CATCH_2S_BTN,
    
    _BUTCOUNT
};


// MISC

#define STATUS_LED 13

#define PUMP_RLY 48