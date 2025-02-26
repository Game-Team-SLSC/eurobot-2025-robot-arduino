#pragma once

#include <DynamicState.h>
#include <Logger.h>
#include <Arduino.h>

#define MAX_STATES 100

class DynamicStateController {
    private:

    DynamicStateController();

    DynamicStateController(const DynamicStateController&) = delete;
    DynamicStateController& operator=(const DynamicStateController&) = delete;

    DynamicStateBase* states[MAX_STATES];
    byte stateCount;
    
    public:

    static DynamicStateController& getInstance();

    ~DynamicStateController();
    
    // Returns a new state
    template <typename T>
    DynamicState<T>* createState(T value);

    // Reset states. Must be called at the end of the main loop
    void resetStates();
};

// Template function definition
template <typename T>
DynamicState<T>* DynamicStateController::createState(T value) {
    if (stateCount >= MAX_STATES) {
        error("State stack overflow");
        return nullptr;
    };

    DynamicState<T>* state = new DynamicState<T>(value);
    states[stateCount] = state;
    ++stateCount;

    return state;
}

// Inline function definition
template <typename T>
inline DynamicState<T>* createState(T value) {
    return DynamicStateController::getInstance().createState(value);
}