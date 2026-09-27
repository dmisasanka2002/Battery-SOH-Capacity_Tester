#ifndef CONFIG_H
#define CONFIG_H


#include <stdint.h>
#include <Adafruit_ADS1X15.h>


#define SERIAL_BAUD 115200

// ============================================================
// GLOBAL TIMING
// ============================================================

constexpr uint32_t LOG_INTERVAL_MS = 100;


// ============================================================
// ADS1115 CONFIGURATION
// ============================================================

constexpr adsGain_t ADS1115_GAIN = GAIN_ONE;

constexpr uint16_t ADC_SAMPLES_PER_MEASUREMENT = 20;

constexpr uint16_t ADC_SAMPLE_DELAY_MS = 5;


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

#endif