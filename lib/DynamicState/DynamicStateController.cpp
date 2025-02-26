#include <DynamicStateController.h>

DynamicStateController& DynamicStateController::getInstance() {
    static DynamicStateController instance;
    return instance;
}

DynamicStateController::DynamicStateController(): stateCount(0) {}

DynamicStateController::~DynamicStateController() {
    for (byte i = 0; i < stateCount; ++i) {
        delete states[i];
    }
}

void DynamicStateController::resetStates() {
    for (byte i = 0; i < stateCount; ++i) {
        states[i]->reset();
    }
}