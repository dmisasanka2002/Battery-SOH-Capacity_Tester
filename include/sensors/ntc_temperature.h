#pragma once

#include <Arduino.h>

float readBatteryTemperature();
float readEnvironmentTemperature();

bool isTemperatureValid(float temperatureC);