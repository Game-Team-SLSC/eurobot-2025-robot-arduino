#pragma once

#include <RemoteData.h>

class Auto {
    public:

    static void fetchData(RemoteData& remoteData);

    private:

    static RemoteData emulatedData;

    static void pressButton(byte button);
    static void runGameStart();
};