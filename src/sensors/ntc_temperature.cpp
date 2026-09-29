#include "sensors/ntc_temperature.h"

#include <math.h>

#include "Config.h"
#include "Pins.h"
#include "sensors/ads1115.h"

static float millivoltsToCelsius(float milliVolts)
{
    float adcVolts = milliVolts / 1000.0f;

    if (adcVolts <= 0.0f || adcVolts >= NTC_SUPPLY_VOLTAGE)
    {
        return NAN;
    }

    float ntcResistance =
        NTC_SERIES_RESISTOR_OHMS * adcVolts / (NTC_SUPPLY_VOLTAGE - adcVolts);

    float kelvin = 1.0f / (
        (1.0f / 298.15f) +
        (1.0f / NTC_BETA) * log(ntcResistance / NTC_R25_OHMS)
    );

    return kelvin - 273.15f;
}

static float readNtcTemperatureC(uint8_t adcChannel)
{
    int32_t totalMillivolts = 0;

    for (uint16_t sampleIndex = 0; sampleIndex < ADC_SAMPLES_PER_MEASUREMENT; sampleIndex++)
    {
        totalMillivolts += static_cast<int32_t>(adcManager.readMillivolts(adcChannel));
        delay(ADC_SAMPLE_DELAY_MS);
    }

    float averageMillivolts =
        static_cast<float>(totalMillivolts) / ADC_SAMPLES_PER_MEASUREMENT;

    return millivoltsToCelsius(averageMillivolts);
}

float readBatteryTemperature()
{
    return readNtcTemperatureC(NTC_ADC_CHANNEL);
}

float readEnvironmentTemperature()
{
    return readNtcTemperatureC(ENVIRONMENT_NTC_ADC_CHANNEL);
}

bool isTemperatureValid(float temperatureC)
{
    return !isnan(temperatureC) &&
           temperatureC >= MIN_EXPECTED_TEMPERATURE_C &&
           temperatureC <= MAX_EXPECTED_TEMPERATURE_C;
}