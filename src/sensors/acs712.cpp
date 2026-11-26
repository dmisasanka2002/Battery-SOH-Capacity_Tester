#include "sensors/acs712.h"

#include <math.h>

#include "Config.h"
#include "Pins.h"
#include "sensors/ads1115.h"

static float zeroVoltage = ACS712_ZERO_CURRENT_VOLTAGE;   // sensor output volts at 0 A
static bool zeroCalibrated = false;

// Average ADS1115 pin voltage in volts. Returns NAN when the input
// sits at the bottom of the range or is saturated.
static float readAveragedPinVolts(uint16_t samples)
{
    float totalMillivolts = 0.0f;

    for (uint16_t i = 0; i < samples; i++)
    {
        totalMillivolts += adcManager.readMillivolts(CURRENT_ADC_CHANNEL);
        delay(ADC_SAMPLE_DELAY_MS);
    }

    float pinVolts = (totalMillivolts / samples) / 1000.0f;

    if (pinVolts < 0.02f || pinVolts > 4.05f) return NAN;

    return pinVolts;
}

bool acs712Calibrate()
{
    float pinVolts = readAveragedPinVolts(ACS712_ZERO_CAL_SAMPLES);

    if (isnan(pinVolts)) return false;

    float outputVolts = pinVolts / ACS712_DIVIDER_RATIO;

    if (fabsf(outputVolts - ACS712_ZERO_CURRENT_VOLTAGE) > ACS712_ZERO_VALID_TOLERANCE_V)
    {
        return false;
    }

    zeroVoltage = outputVolts;
    zeroCalibrated = true;
    return true;
}

float acs712GetZeroVoltage()
{
    return zeroVoltage;
}

float readBatteryCurrent()
{
    if (!zeroCalibrated) return NAN;
    float pinVolts = readAveragedPinVolts(ADC_SAMPLES_PER_MEASUREMENT);

    if (isnan(pinVolts)) return NAN;

    float outputVolts = pinVolts / ACS712_DIVIDER_RATIO;

    return ACS712_CURRENT_SIGN * (outputVolts - zeroVoltage) / ACS712_SENSITIVITY_V_PER_A;
}