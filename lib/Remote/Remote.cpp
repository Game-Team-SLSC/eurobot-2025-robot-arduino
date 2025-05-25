#include <Remote.h>
#include <GlobalSettings.h>
#include <SecretSettings.h>

RF24 Remote::radio(RF_CE, RF_CS);
unsigned long long Remote::lastReceiveTime = 0;

// yields
void Remote::setup() {
    while (!radio.begin()) {
        delay(RF_RETRY_DELAY);
    }

    radio.openReadingPipe(1, (const byte*)RF_ADDRESS);
    radio.setPALevel(RF24_PA_LOW);
    radio.setDataRate(RF24_250KBPS);
    radio.setChannel(RADIO_CHANNEL);

    radio.startListening();

    #ifdef INFO_STATE
    radio.printPrettyDetails();
    #endif
}

/*
@returns Weither the data was available
*/
bool Remote::fetch(RemoteData& dataBuffer) {
    if (!radio.available()) {
        if ((millis() - lastReceiveTime) >= RF_TIMEOUT) {
            GlobalState::remoteConnected->set(false);
        }   
        return false;
    }

    GlobalState::remoteConnected->set(true);

    lastReceiveTime = millis();
    radio.read(&dataBuffer, sizeof(dataBuffer));

    return true;
}