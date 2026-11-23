#pragma once

#include <Arduino.h>

struct LiveReading
{
    String timestamp;
    uint32_t sampleNumber;
    float voltage;
    float current;
    float batteryTemperature;
    float environmentTemperature;
};

// Takes one real sample every LOG_INTERVAL_MS. Returns true when a new sample was taken.
bool liveMonitorUpdate(LiveReading& reading);

void liveMonitorPrint(const LiveReading& reading, const String& method);