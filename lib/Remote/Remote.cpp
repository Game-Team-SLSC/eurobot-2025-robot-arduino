#include <Remote.h>


RF24 Remote::radio(RF_CE, RF_CS);
BaseTimer* Remote::timeoutTimer(nullptr);

// yields
void Remote::setup() {
    while (!radio.begin()) {
        delay(RF_RETRY_DELAY);
    }

    radio.openReadingPipe(1, (const byte*)RF_ADDRESS);
    radio.setPALevel(RF24_PA_LOW);
    radio.setDataRate(RF24_250KBPS);
    radio.setChannel(110);

    radio.startListening();

    #ifdef INFO_STATE
    radio.printPrettyDetails();
    #endif
}

/*
@returns Weither the data was available
async
*/
bool Remote::fetch(RemoteData& dataBuffer) {
    if (!radio.available()) {
        if (timeoutTimer == nullptr) {
            timeoutTimer = Timing::in(RF_TIMEOUT, +[](void* selfPtr) {
                Remote* self = static_cast<Remote*>(selfPtr);
                self->timeoutTimer = nullptr;
                GlobalState::remoteConnected->set(false);
            GlobalState::remoteConnected->set(false);
        }, nullptr);
    }

        return false;
    };

    GlobalState::remoteConnected->set(true);

    if (timeoutTimer != nullptr) {
        Timing::cancelTimer(timeoutTimer);
        timeoutTimer = nullptr;
    }

    radio.read(&dataBuffer, sizeof(dataBuffer));

    return true;
}