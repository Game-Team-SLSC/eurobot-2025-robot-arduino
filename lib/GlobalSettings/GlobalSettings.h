#pragma once

// DEBUG

//#define MOCK_REMOTE

//#define LOG_STATE

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

#define RF_CE A3
#define RF_CS A2
#define RF_TIMEOUT 1000
#define RF_ADDRESS "CR912"
#define RF_RETRY_DELAY 1000

// ESP 32

#define ESP_SERIAL Serial1
#define ESP_BAUD 9600

// SCORE

#define BLINK_INTERVAL 500
#define DEFAULT_SCORE 66
#define SCORE_DP_DIO A0
#define SCORE_DP_CLK A1

// MOVERS

#define MOVERS_RAMPING 0.2f
#define MOVERS_TIMEOUT 1000
#define DEADZONE 3

#define YAW_FACTOR 0.55f
#define X_MOVE_FACTOR .8f
#define Y_MOVE_FACTOR .8f

#define APPROACH_SPEED_FACTOR 0.3f
#define S2_SPEED_FACTOR 0.3f
#define TRANSPORT_SPEED_FACTOR 0.6f

#define SBR_L 46
#define SBR_R 5

#define MOVER_RL 2
#define MOVER_FL 1
#define MOVER_RR 2
#define MOVER_FR 1

// ACTUATORS

#define PCA9685_ADDR 0x40

#define GRB_L_PIN 8
#define GRB_RELEASE_ANGLE_L 70
#define GRB_CATCH_ANGLE_L 80

#define GRB_R_PIN 7
#define GRB_RELEASE_ANGLE_R 110
#define GRB_CATCH_ANGLE_R 100

#define GRB_MAGNET_L_PIN 2
#define GRB_MAGNET_ATTACH_ANGLE_L 73
#define GRB_MAGNET_RELEASE_ANGLE_L 133

#define GRB_MAGNET_R_PIN 15
#define GRB_MAGNET_ATTACH_ANGLE_R 150
#define GRB_MAGNET_RELEASE_ANGLE_R 80

#define GRB_ARM_L_PIN 0
#define GRB_ARM_DEP_ANGLE_L 86
#define GRB_ARM_REL_ANGLE_L 93
#define GRB_ARM_RET_ANGLE_L 140

#define GRB_ARM_R_PIN 12
#define GRB_ARM_DEP_ANGLE_R 54
#define GRB_ARM_REL_ANGLE_R 47
#define GRB_ARM_RET_ANGLE_R 0

#define SC_L_PIN 6
#define SC_DEP_ANGLE_L 237
#define SC_LIFT_ANGLE_L 212
#define SC_RET_ANGLE_L 147

#define SC_R_PIN 4
#define SC_DEP_ANGLE_R 60
#define SC_LIFT_ANGLE_R 85
#define SC_RET_ANGLE_R 155

#define BANNER_PIN 11
#define BANNER_DEP_ANGLE 150
#define BANNER_RET_ANGLE 80

// BUTTONS CONFIG

enum Buttons {
    // Left side
    EXTRACT_STAGE_BTN,
    STAGES_2_AUTO_BTN,
    STAGES_3_AUTO_BTN,
    APPROACH_BTN,

    // Right side (down)
    TRANSPORT_BTN,
    RELEASE_BTN,

    // RB button
    STAGE_1_BTN,

    // Right side (up)
    RELEASE_BANNER_BTN,
    FOLD_BTN,
    
    // Up and down
    STAGE_2_BTN,
    
    // Secondary actions
    CATCH_BTN,
    
    _BUTCOUNT
};


// MISC

#define STATUS_LED LED_BUILTIN

#define PUMP_RLY 48