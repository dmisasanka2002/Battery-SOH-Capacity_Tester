#include <Arduino.h>
#include <Wire.h>

#include "Config.h"
#include "Pins.h"

#include "sensors/ads1115.h"
#include "sensors/voltage_sensor.h"

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Wire.begin(
        I2C_SDA_PIN,
        I2C_SCL_PIN);

    Serial.println();
    Serial.println(
        "================================");

    Serial.println(
        "BATTERY VOLTAGE SENSOR");

    Serial.println(
        "================================");

    if (!adcManager.begin(
            ADS1115_I2C_ADDRESS))
    {
        Serial.println(
            "ERROR: ADS1115 not detected.");

        while (true)
        {
            delay(1000);
        }
    }

    Serial.println(
        "ADS1115 initialized.");
}

void loop()
{
    int32_t rawAdc =
        readVoltageRaw();

    float batteryVoltage =
        convertRawAdcToVoltage(
            rawAdc);

    Serial.print(
        "Raw ADC = ");

    Serial.print(rawAdc);

    Serial.print(
        " | Battery Voltage = ");

    Serial.print(
        batteryVoltage,
        6);

    Serial.print(
        " V | Status = ");

    if (
        isBatteryVoltageValid(
            batteryVoltage))
    {
        Serial.println("OK");
    }
    else
    {
        Serial.println(
            "OUT_OF_EXPECTED_RANGE");
    }

    delay(
        LOG_INTERVAL_MS);
}