#include <Logger.h>
#include <printf.h>

void loggerSetup() {
    Serial.begin(9600);
    #ifdef INFO_STATE
    printf_begin();
    #endif
}

void info(String msg) {
    #ifndef INFO_STATE
    return;
    #endif
    
    Serial.println(INFO_TAG + msg);
}

void warn(String msg) {
    #ifndef WARN_STATE
    return;
    #endif

    Serial.println(WARN_TAG + msg);
}

extern void error(String msg) {
    #ifndef ERROR_STATE
    return;
    #endif

    Serial.println(ERROR_TAG + msg);
}