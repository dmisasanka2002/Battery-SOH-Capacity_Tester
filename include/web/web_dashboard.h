#pragma once

#include <Arduino.h>

// ============================================================
// WEB DASHBOARD
// ============================================================

void webDashboardBegin();

void webDashboardHandle();


// ============================================================
// LIVE MEASUREMENT UPDATE
// ============================================================

void webDashboardUpdate(
    float voltage,
    float current,
    float batteryTemperature,
    float environmentTemperature
);