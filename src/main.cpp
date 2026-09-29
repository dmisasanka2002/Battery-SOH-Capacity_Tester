#include <Arduino.h>
#include <Wire.h>

#include "Config.h"
#include "Pins.h"

#include "sensors/ads1115.h"
#include "sensors/voltage_sensor.h"
#include "sensors/ina226.h"

#include "rtc/rtc_manager.h"
#include "control/test_controller.h"
#include "storage/sd_logger.h"
#include "web/web_dashboard.h"

void setup()
{
    Serial.begin(SERIAL_BAUD);
    delay(1000);

    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

    Serial.println("================================");
    Serial.println("BATTERY CELL HEALTH MONITOR");
    Serial.println("================================");

    if (!rtcBegin())
    {
        Serial.println("WARNING: continuing without RTC. Timestamps will be invalid.");
    }

    if (!adcManager.begin(ADS1115_I2C_ADDRESS))
    {
        Serial.println("ERROR: ADS1115 not detected.");
        while (true) delay(1000);
    }

    // if (!currentSensor.begin(INA226_I2C_ADDRESS))
    // {
    //     Serial.println("ERROR: INA226 not detected.");
    //     while (true) delay(1000);
    // }

    if (!sdLogger.begin())
    {
        Serial.println("ERROR: SD card not detected.");
        while (true) delay(1000);
    }

    testControllerBegin();
    webDashboardBegin();

    Serial.println("READY.");
}

void loop()
{
    webDashboardHandle();
    testControllerUpdate();

    delay(2);
}