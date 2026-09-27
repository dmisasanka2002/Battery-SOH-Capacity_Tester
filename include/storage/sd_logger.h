#pragma once

#include <Arduino.h>
#include <FS.h>

#include "csv_format.h"
#include "file_manager.h"


// ============================================================
// SD LOGGER
// ============================================================

class SDLogger
{
public:

    bool begin();


    // --------------------------------------------------------
    // Create a new test file.
    //
    // If the requested filename already exists,
    // an _01, _02, ... suffix is automatically selected.
    // --------------------------------------------------------

    bool startTestFile(
        const String& date,
        uint16_t moduleNumber,
        TestMode mode,
        uint32_t cycleNumber
    );


    // --------------------------------------------------------
    // Write one CSV record.
    // --------------------------------------------------------

    bool logRecord(
        const DataRecord& record
    );


    // --------------------------------------------------------
    // Close current file.
    // --------------------------------------------------------

    void closeFile();


    // --------------------------------------------------------
    // Status
    // --------------------------------------------------------

    bool isFileOpen();


    // --------------------------------------------------------
    // Current file path
    // --------------------------------------------------------

    String getCurrentFilePath();


private:

    String generateUniqueFilePath(
        const String& basePath
    );


    File currentFile;

    String currentFilePath;
};


// Global logger.

extern SDLogger sdLogger;