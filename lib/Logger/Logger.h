#pragma once

#include <GlobalSettings.h>
#include <Arduino.h>

void loggerSetup();
void info(String msg);
void warn(String msg);
void error(String msg);