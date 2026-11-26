#include <Arduino.h>
#include <Wire.h>

#include "Config.h"
#include "Pins.h"

#include "sensors/ads1115.h"
#include "sensors/voltage_sensor.h"
#include "sensors/acs712.h"

#include "rtc/rtc_manager.h"
#include "control/test_controller.h"
#include "storage/sd_logger.h"
#include "web/web_dashboard.h"
#include "sensors/live_monitor.h"

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

    Serial.printf("ACS712 zero-current voltage: %.4f V\n", acs712GetZeroVoltage());

        Serial.printf("ACS712 pin voltage (A1): %.1f mV\n",
                  adcManager.readMillivolts(CURRENT_ADC_CHANNEL));

    if (acs712Calibrate())
    {
        Serial.printf("ACS712 zero-current voltage: %.4f V\n", acs712GetZeroVoltage());
    }
    else
    {
        Serial.println("WARNING: ACS712 zero check failed. Current will show nan.");
        Serial.println("Expected about 2500 mV on A1 with no current flowing.");
    }

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

    LiveReading reading;

    if (liveMonitorUpdate(reading))
    {
        webDashboardUpdate(reading.voltage,
                           reading.current,
                           reading.batteryTemperature,
                           reading.environmentTemperature);

        // Print before the controller runs so the printed method
        // matches the method written to the CSV row.
        liveMonitorPrint(reading, testControllerGetCurrentMethodName());

        testControllerUpdate(reading);
    }

    delay(2);
}