#pragma once

#include <Arduino.h>

int32_t readVoltageRaw();

float convertRawAdcToVoltage(
    int32_t rawAdc
);

float readBatteryVoltage();

bool isBatteryVoltageValid(
    float voltage
);