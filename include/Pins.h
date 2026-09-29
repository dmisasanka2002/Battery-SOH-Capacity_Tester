#ifndef PINS_H
#define PINS_H

#include <stdint.h>

// ============================================================
// I2C
// ============================================================

constexpr uint8_t I2C_SDA_PIN = 21;
constexpr uint8_t I2C_SCL_PIN = 22;


// ============================================================
// ADS1115
// ============================================================

constexpr uint8_t ADS1115_I2C_ADDRESS = 0x48;


// ============================================================
// ADS1115 CHANNEL ASSIGNMENT
// ============================================================

constexpr uint8_t VOLTAGE_ADC_CHANNEL = 0;

// NTC channel will be added later.
constexpr uint8_t NTC_ADC_CHANNEL = 2;

constexpr uint8_t ENVIRONMENT_NTC_ADC_CHANNEL = 1;   // confirm this wiring — channel 1 is currently free


#define RX_PIN 16
#define TX_PIN 17

#define RELAY_CH1 26
#define RELAY_CH2 27

#define SD_CS_PIN 5
#define SD_SCK 18
#define SD_MISO 19
#define SD_MOSI 23


#endif