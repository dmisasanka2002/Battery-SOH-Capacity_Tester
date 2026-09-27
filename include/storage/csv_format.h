#pragma once

#include <Arduino.h>

// ============================================================
// CSV DATA STRUCTURE
// ============================================================

struct DataRecord
{
    String timestamp;

    uint16_t moduleNumber;

    float voltage;
    float current;

    float batteryTemperature;
    float environmentTemperature;

    String mode;
    String method;

    uint32_t cycleNumber;
};


// ============================================================
// CSV FORMAT FUNCTIONS
// ============================================================

// Returns the CSV header.
String getCsvHeader();

// Converts one DataRecord into one CSV row.
String getCsvRow(
    const DataRecord& record
);