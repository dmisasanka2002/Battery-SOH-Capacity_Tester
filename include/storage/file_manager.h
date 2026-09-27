#pragma once

#include <Arduino.h>


// ============================================================
// TEST MODE
// ============================================================

enum class TestMode
{
    CapacityTest,
    CyclingTest
};


// ============================================================
// MODE → FILENAME TEXT
// ============================================================

String testModeToFilename(
    TestMode mode
);


// ============================================================
// MODULE FOLDER
//
// Example:
// /Module_001
// ============================================================

String getModuleFolder(
    uint16_t moduleNumber
);


// ============================================================
// BASE TEST FILENAME
//
// Example:
// 2026-09-27_Module_001_Capacity_Test_Cycle_1.csv
// ============================================================

String getTestFilename(
    const String& date,
    uint16_t moduleNumber,
    TestMode mode,
    uint32_t cycleNumber
);


// ============================================================
// BASE COMPLETE PATH
//
// Example:
// /Module_001/
// 2026-09-27_Module_001_Capacity_Test_Cycle_1.csv
// ============================================================

String getTestFilePath(
    const String& date,
    uint16_t moduleNumber,
    TestMode mode,
    uint32_t cycleNumber
);

// ============================================================
// SD FILE MANAGEMENT
// ============================================================

// Create module folder if it does not already exist.
bool ensureModuleFolder(
    uint16_t moduleNumber
);


// Check whether a file exists.
bool fileExists(
    const String& path
);


// Generate a non-conflicting filename.
//
// Example:
//
// Original:
// 2026-09-27_Module_001_Capacity_Test_Cycle_1.csv
//
// If already exists:
//
// 2026-09-27_Module_001_Capacity_Test_Cycle_1_01.csv
//
String getUniqueFilePath(
    const String& requestedPath
);


// Validate a requested download path.
bool isValidCsvPath(
    const String& path
);


// Return all CSV files as JSON for the web dashboard.
String getCsvFileListJson();