#pragma once

#include <Arduino.h>
#include <Adafruit_ADS1X15.h>

class Ads1115Wrapper
{
private:
    Adafruit_ADS1115 ads;

    bool isInitialized;

public:
    Ads1115Wrapper();

    bool begin(
        uint8_t i2cAddress = 0x48);

    int16_t readRaw(
        uint8_t channel);

    float readMillivolts(
        uint8_t channel);
};

extern Ads1115Wrapper adcManager;