#include <Logger.h>
#include <printf.h>

void loggerSetup() {
    Serial.begin(9600);
    printf_begin();
}