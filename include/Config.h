#ifndef CONFIG_H
#define CONFIG_H


#include <stdint.h>
#include <Adafruit_ADS1X15.h>


#define SERIAL_BAUD 115200

constexpr bool SERIAL_LIVE_PRINT = true;
constexpr uint32_t SERIAL_PRINT_EVERY_N_SAMPLES = 1;   // 1 = print every 100 ms sample

// A floating ADS1115 input reads random values, not zero.


// ============================================================
// GLOBAL TIMING
// ============================================================

constexpr uint32_t LOG_INTERVAL_MS = 100;

constexpr bool ENVIRONMENT_NTC_INSTALLED = false;

// ACS712 current sensor on ADS1115 channel CURRENT_ADC_CHANNEL.
// Sensitivity: 5 A part 0.185 V/A, 20 A part 0.100 V/A, 30 A part 0.066 V/A.
constexpr float ACS712_SENSITIVITY_V_PER_A = 0.185f;

// Output at 0 A is half the ACS712 supply (2.5 V on a 5 V supply).
constexpr float ACS712_ZERO_CURRENT_VOLTAGE = 2.500f;
constexpr float ACS712_ZERO_VALID_TOLERANCE_V = 0.200f;
constexpr uint16_t ACS712_ZERO_CAL_SAMPLES = 200;

// Divider between the ACS712 output and the ADS1115 pin: R2 / (R1 + R2).
// Use 1.0f when the output goes straight to the ADS1115.
constexpr float ACS712_DIVIDER_RATIO = 1.0f;

// Use -1.0f if charging reads negative and discharging reads positive.
constexpr float ACS712_CURRENT_SIGN = 1.0f;

// ============================================================
// ADS1115 CONFIGURATION
// ============================================================

constexpr adsGain_t ADS1115_GAIN = GAIN_ONE;

constexpr uint16_t ADC_SAMPLES_PER_MEASUREMENT = 5;

constexpr uint16_t ADC_SAMPLE_DELAY_MS = 1;



// ============================================================
// VOLTAGE SENSOR CALIBRATION
// ============================================================
//
// Vbattery = slope * RawADC + intercept
//
// Replace these with your actual calibrated values.
//

constexpr float VOLTAGE_CALIBRATION_SLOPE =
    0.000396f;

constexpr float VOLTAGE_CALIBRATION_INTERCEPT =
    -0.010184f;


// ============================================================
// VOLTAGE SAFETY LIMITS
// ============================================================

constexpr float MIN_EXPECTED_BATTERY_VOLTAGE =
    0.0f;

constexpr float MAX_EXPECTED_BATTERY_VOLTAGE =
    12.0f;



// INA226 current sensor
// constexpr uint8_t INA226_I2C_ADDRESS = 0x40;
// constexpr float INA226_SHUNT_RESISTOR_OHMS = 0.1f;      // match your shunt
// constexpr float INA226_MAX_EXPECTED_CURRENT_A = 5.0f;   // match your test current

// NTC thermistors (voltage divider: Vcc -> R_SERIES -> ADC node -> NTC -> GND)
constexpr float NTC_SUPPLY_VOLTAGE = 3.3f;
constexpr float NTC_SERIES_RESISTOR_OHMS = 10000.0f;
constexpr float NTC_R25_OHMS = 10000.0f;
constexpr float NTC_BETA = 3950.0f;
constexpr float MIN_EXPECTED_TEMPERATURE_C = -10.0f;
constexpr float MAX_EXPECTED_TEMPERATURE_C = 80.0f;

// Wi-Fi access point
constexpr char WIFI_AP_SSID[] = "BatteryTester";
constexpr char WIFI_AP_PASSWORD[] = "battery123";

// Test sequence
constexpr uint8_t MAX_SEQUENCE_LENGTH = 10;
constexpr uint32_t METHOD_STEP_DURATION_MS = 10000;   // placeholder for testing, will be replaced by actual duration in the test method

#endif