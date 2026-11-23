#include "sensors/live_monitor.h"

#include "Config.h"
#include "rtc/rtc_manager.h"
#include "sensors/voltage_sensor.h"
#include "sensors/acs712.h"      // was sensors/ina226.h
#include "sensors/ntc_temperature.h"

static uint32_t nextSampleMillis = 0;
static uint32_t sampleCounter = 0;

bool liveMonitorUpdate(LiveReading& reading)
{
    uint32_t now = millis();

    if (static_cast<int32_t>(now - nextSampleMillis) < 0) return false;

    // Fixed schedule so the interval does not drift by the sensor read time.
    nextSampleMillis += LOG_INTERVAL_MS;

    if (static_cast<int32_t>(now - nextSampleMillis) >= 0)
    {
        nextSampleMillis = now + LOG_INTERVAL_MS;   // fell behind, resync
    }

    reading.timestamp = getTimestamp();
    reading.sampleNumber = ++sampleCounter;
    reading.voltage = readBatteryVoltage();
    reading.current = readBatteryCurrent();
    reading.batteryTemperature = readBatteryTemperature();
    reading.environmentTemperature = readEnvironmentTemperature();

    return true;
}

void liveMonitorPrint(const LiveReading& reading, const String& method)
{
    if (!SERIAL_LIVE_PRINT) return;
    if (reading.sampleNumber % SERIAL_PRINT_EVERY_N_SAMPLES != 0) return;

    Serial.printf(
        "%s | #%lu | V=%.4f V | I=%.4f A | Tb=%.2f C | Te=%.2f C | %s\n",
        reading.timestamp.c_str(),
        static_cast<unsigned long>(reading.sampleNumber),
        reading.voltage,
        reading.current,
        reading.batteryTemperature,
        reading.environmentTemperature,
        method.c_str());
}