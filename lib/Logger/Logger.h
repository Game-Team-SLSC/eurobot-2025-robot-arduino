#pragma once

#include <GlobalSettings.h>
#include <Arduino.h>

void loggerSetup();

#ifdef INFO_STATE

#define info(msg, ...) \
printf(msg, ##__VA_ARGS__); \
Serial.println()

#else

#define info(msg, ...)

#endif

#ifdef WARN_STATE

#define warn(msg, ...) \
printf(msg, ##__VA_ARGS__); \
Serial.println()

#else

#define warn(msg, ...)

#endif

#ifdef ERROR_STATE

#define error(msg, ...) \
printf(msg, ##__VA_ARGS__); \
Serial.println()

#else

#define error(msg, ...)

#endif