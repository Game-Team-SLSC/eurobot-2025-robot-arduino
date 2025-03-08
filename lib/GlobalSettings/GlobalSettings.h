#pragma once

// WARNING : A TOO LOW VALUE MAY BLOCK PROGRAM TRANSFER (AVRDUDE: stk500_getsync() attempt 10 of 10: not in sync: resp=0x00)
// values between 50 and 200 are recommended. Increasing the value will diminish the responsiveness of the robot.

#define LOOP_DELAY 50

// DEBUG

#define MOCK_REMOTE

#define LOG_STATE
#define ERROR_STATE
#define WARN_STATE
#define INFO_STATE

#define INFO_TAG "\033[34m[INFO] - "
#define WARN_TAG "\033[33m[WARN] - "
#define ERROR_TAG "\033[31m[ERROR] - "

// RF24

#define RF_CE 48
#define RF_CS 49
#define RF_MISO 50
#define RF_MOSI 51
#define RF_SCK 52
#define RF_IRQ 2
#define RF_TIMEOUT 1000
#define RF_ADDRESS "CR912"
#define RF_RETRY_DELAY 1000

// STEPPERS

#define GRB_UP_HEIGHT 100 //mm
#define GRB_STEP 34
#define GRB_DIR 36
#define GRB_SERIAL Serial2

#define SC_UP_HEIGHT 100 //mm
#define SC_STEP 38
#define SC_DIR 40
#define SC_SERIAL Serial3

#define R_SENSE 0.11f
#define STEPS_PER_MM 45


// SCORE

#define BLINK_INTERVAL 500
#define DEFAULT_SCORE 70
#define SCORE_DP_DIO 26
#define SCORE_DP_CLK 27

// MOVERS

#define MOVERS_RAMPING 45
#define MOVERS_TIMEOUT 1000

#define YAW_FACTOR 100
#define X_MOVE_FACTOR 255
#define Y_MOVE_FACTOR 255

#define FL_EN 4
#define FL_IN1 5
#define FL_IN2 6

#define FR_EN 9
#define FR_IN1 7
#define FR_IN2 8

#define RL_EN 45
#define RL_IN1 46
#define RL_IN2 47

#define RR_EN 44
#define RR_IN1 42
#define RR_IN2 43

#define APPROACH_SPEED_FACTOR 0.3

// ACTUATORS

#define GRB_L_PIN 22
#define GRB_RELEASE_ANGLE_L 85
#define GRB_CATCH_ANGLE_L 95

#define GRB_R_PIN 23
#define GRB_RELEASE_ANGLE_R 90
#define GRB_CATCH_ANGLE_R 80

#define GRB_MAGNET_L_PIN 24
#define GRB_MAGNET_ATTACH_ANGLE_L 0
#define GRB_MAGNET_RELEASE_ANGLE_L 90

#define GRB_MAGNET_R_PIN 25
#define GRB_MAGNET_ATTACH_ANGLE_R 180
#define GRB_MAGNET_RELEASE_ANGLE_R 90

#define GRB_ARM_L_PIN 26
#define GRB_ARM_DEP_ANGLE_L 110
#define GRB_ARM_RET_ANGLE_L 180

#define GRB_ARM_R_PIN 27
#define GRB_ARM_DEP_ANGLE_R 80
#define GRB_ARM_RET_ANGLE_R 10

#define SC_L_PIN 28
#define SC_DEP_ANGLE_L 180
#define SC_RET_ANGLE_L 60

#define SC_R_PIN 29
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
    STAGE_1_BTN,
    STAGE_2_BTN,
    FOLD_BTN,
    SLOW_DOWN_BTN,
    
    RIGHT_SIDE_BTN,
    LEFT_SIDE_BTN,

    CATCH_2S_BTN,

    _BUTCOUNT
};

// MISC

#define STATUS_LED 13

#define PUMP_RLY 32