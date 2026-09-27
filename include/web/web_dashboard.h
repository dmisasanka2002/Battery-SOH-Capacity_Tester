#pragma once

#include <Arduino.h>

// ============================================================
// WEB DASHBOARD
// ============================================================

void webDashboardBegin();

void webDashboardHandle();


// ============================================================
// LIVE MEASUREMENTS
// ============================================================

void webDashboardUpdate(
    float voltage,
    float current,
    float batteryTemperature,
    float environmentTemperature
);