#pragma once

#include <GlobalSettings.h>
#include <Arduino.h>

#ifdef LOG_STATE

void loggerSetup();

#else

#define loggerSetup()

#endif

#ifdef INFO_STATE

#define info(msg, ...) \
printf(INFO_TAG); \
printf(msg, ##__VA_ARGS__); \
Serial.println()

#else

#define info(msg, ...)

#endif

#ifdef WARN_STATE

#define warn(msg, ...) \
printf(WARN_TAG); \
printf(msg, ##__VA_ARGS__); \
Serial.println()

#else

#define warn(msg, ...)

#endif

#ifdef ERROR_STATE

#define error(msg, ...) \
printf(ERROR_TAG); \
printf(msg, ##__VA_ARGS__); \
Serial.println()

#else

#define error(msg, ...)

#endif