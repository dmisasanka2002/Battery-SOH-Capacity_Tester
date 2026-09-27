#include "sensors/voltage_sensor.h"

#include <Arduino.h>

#include "Config.h"
#include "Pins.h"
#include "sensors/ads1115.h"


// ============================================================
// READ AVERAGED RAW ADC
// ============================================================

int32_t readVoltageRaw()
{
    int64_t totalRawAdc = 0;

    for (
        uint16_t sampleIndex = 0;
        sampleIndex < ADC_SAMPLES_PER_MEASUREMENT;
        sampleIndex++
    )
    {
        totalRawAdc +=
            adcManager.readRaw(
                VOLTAGE_ADC_CHANNEL
            );

        delay(
            ADC_SAMPLE_DELAY_MS
        );
    }

    return static_cast<int32_t>(
        static_cast<float>(totalRawAdc) /
        ADC_SAMPLES_PER_MEASUREMENT
    );
}


// ============================================================
// CALIBRATED RAW ADC → BATTERY VOLTAGE
// ============================================================

float convertRawAdcToVoltage(
    int32_t rawAdc
)
{
    return
        VOLTAGE_CALIBRATION_SLOPE *
        static_cast<float>(rawAdc)
        +
        VOLTAGE_CALIBRATION_INTERCEPT;
}


// ============================================================
// READ BATTERY VOLTAGE
// ============================================================

float readBatteryVoltage()
{
    int32_t rawAdc =
        readVoltageRaw();

    return convertRawAdcToVoltage(
        rawAdc
    );
}


// ============================================================
// RANGE CHECK
// ============================================================

bool isBatteryVoltageValid(
    float voltage
)
{
    return
        voltage >=
        MIN_EXPECTED_BATTERY_VOLTAGE
        &&
        voltage <=
        MAX_EXPECTED_BATTERY_VOLTAGE;
}