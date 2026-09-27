#include "sensors/ads1115.h"

#include <Wire.h>

#include "Config.h"
#include "Pins.h"

Ads1115Wrapper::Ads1115Wrapper()
{
    isInitialized = false;
}

bool Ads1115Wrapper::begin(
    uint8_t i2cAddress)
{
    if (!ads.begin(
            i2cAddress,
            &Wire))
    {
        isInitialized = false;
        return false;
    }

    ads.setGain(
        ADS1115_GAIN);

    isInitialized = true;

    return true;
}

int16_t Ads1115Wrapper::readRaw(
    uint8_t channel)
{
    if (!isInitialized)
    {
        return 0;
    }

    if (channel > 3)
    {
        return 0;
    }

    return ads.readADC_SingleEnded(
        channel);
}

float Ads1115Wrapper::readMillivolts(
    uint8_t channel)
{
    if (!isInitialized)
    {
        return 0.0f;
    }

    int16_t raw =
        readRaw(channel);

    return ads.computeVolts(raw) * 1000.0f;
}

// ============================================================
// GLOBAL INSTANCE
// ============================================================

Ads1115Wrapper adcManager;