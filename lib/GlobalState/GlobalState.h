#pragma once

#include <Arduino.h>
#include <DynamicStateController.h>
#include <ActionName.h>
#include <GlobalSettings.h>
#include <RemoteData.h>
#include <Travel.h>
#include <RunMode.h>

class GlobalState {
    public:

    static void update(RemoteData& remoteData, RemoteData& emulatedData);

    static DynamicState<ActionName>* action;
    static DynamicState<byte>* score;
    static DynamicState<Travel>* travel;
    static DynamicState<float>* speedFactor;
    static DynamicState<bool>* remoteConnected;
    static DynamicState<RunMode>* runMode;
    static DynamicState<bool>* isRightSide;

    private: 

    static void updateRaw(RemoteData& data);
};