#include "storage/sd_logger.h"

#include <FS.h>
#include <SD.h>
#include <SPI.h>

#include "Pins.h"


// ============================================================
// BEGIN SD
// ============================================================

bool SDLogger::begin()
{
    if (!SD.begin(SD_CS_PIN))
    {
        return false;
    }

    return true;
}


// ============================================================
// GENERATE UNIQUE FILE PATH
//
// Base:
// /Module_001/
// 2026-09-27_Module_001_Capacity_Test_Cycle_1.csv
//
// If exists:
//
// ...Cycle_1_01.csv
// ...Cycle_1_02.csv
// ...Cycle_1_03.csv
// ============================================================

String SDLogger::generateUniqueFilePath(
    const String& basePath
)
{
    // --------------------------------------------------------
    // First try the requested filename.
    // --------------------------------------------------------

    if (!SD.exists(basePath))
    {
        return basePath;
    }


    // --------------------------------------------------------
    // Separate extension.
    // --------------------------------------------------------

    int extensionPosition =
        basePath.lastIndexOf(".csv");


    if (extensionPosition < 0)
    {
        return basePath;
    }


    String baseWithoutExtension =
        basePath.substring(
            0,
            extensionPosition
        );


    // --------------------------------------------------------
    // Search _01, _02, _03...
    // --------------------------------------------------------

    for (uint16_t suffix = 1;
         suffix <= 9999;
         suffix++)
    {
        String candidate;

        candidate += baseWithoutExtension;

        candidate += "_";


        if (suffix < 10)
        {
            candidate += "0";
        }

        candidate += String(suffix);

        candidate += ".csv";


        if (!SD.exists(candidate))
        {
            return candidate;
        }
    }


    // --------------------------------------------------------
    // No available suffix.
    // --------------------------------------------------------

    return "";
}


// ============================================================
// START TEST FILE
// ============================================================

bool SDLogger::startTestFile(
    const String& date,
    uint16_t moduleNumber,
    TestMode mode,
    uint32_t cycleNumber
)
{
    // --------------------------------------------------------
    // Close previous file if necessary.
    // --------------------------------------------------------

    closeFile();


    // --------------------------------------------------------
    // Module folder.
    // --------------------------------------------------------

    String moduleFolder =
        getModuleFolder(
            moduleNumber
        );


    // --------------------------------------------------------
    // Create folder if required.
    // --------------------------------------------------------

    if (!SD.exists(moduleFolder))
    {
        if (!SD.mkdir(moduleFolder))
        {
            return false;
        }
    }


    // --------------------------------------------------------
    // Generate requested path.
    // --------------------------------------------------------

    String requestedPath =
        getTestFilePath(
            date,
            moduleNumber,
            mode,
            cycleNumber
        );


    // --------------------------------------------------------
    // Generate unique path.
    // --------------------------------------------------------

    currentFilePath =
        generateUniqueFilePath(
            requestedPath
        );


    if (currentFilePath.length() == 0)
    {
        return false;
    }


    // --------------------------------------------------------
    // Create file.
    // --------------------------------------------------------

    currentFile =
        SD.open(
            currentFilePath,
            FILE_WRITE
        );


    if (!currentFile)
    {
        currentFilePath = "";

        return false;
    }


    // --------------------------------------------------------
    // Write CSV header.
    // --------------------------------------------------------

    currentFile.println(
        getCsvHeader()
    );


    return true;
}


// ============================================================
// WRITE RECORD
// ============================================================

bool SDLogger::logRecord(
    const DataRecord& record
)
{
    if (!currentFile)
    {
        return false;
    }


    String row =
        getCsvRow(record);


    if (!currentFile.println(row))
    {
        return false;
    }


    return true;
}


// ============================================================
// CLOSE FILE
// ============================================================

void SDLogger::closeFile()
{
    if (currentFile)
    {
        currentFile.flush();

        currentFile.close();
    }
}


// ============================================================
// FILE OPEN?
// ============================================================

bool SDLogger::isFileOpen()
{
    return (bool)currentFile;
}


// ============================================================
// CURRENT PATH
// ============================================================

String SDLogger::getCurrentFilePath()
{
    return currentFilePath;
}


// ============================================================
// GLOBAL INSTANCE
// ============================================================

SDLogger sdLogger;