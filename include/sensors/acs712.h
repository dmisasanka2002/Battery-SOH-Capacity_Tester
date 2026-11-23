#pragma once

#include <Arduino.h>

// Measures the zero-current output with no load and stores it.
// Returns false if the reading is outside the expected window.
bool acs712Calibrate();

float acs712GetZeroVoltage();

// Battery current in amps, or NAN when the reading is not valid.
float readBatteryCurrent();