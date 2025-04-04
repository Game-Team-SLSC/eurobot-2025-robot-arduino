#include <Logger.h>
#include <printf.h>

#ifdef LOG_STATE
void loggerSetup() {
    Serial.begin(9600);
    printf_begin();
}
#else
void loggerSetup();
#endif