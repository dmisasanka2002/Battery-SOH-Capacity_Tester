#include "storage/file_manager.h"

#include <SD.h>


// ============================================================
// TEST MODE → TEXT
// ============================================================

String testModeToFilename(
    TestMode mode
)
{
    switch (mode)
    {
        case TestMode::CapacityTest:
            return "Capacity_Test";

        case TestMode::CyclingTest:
            return "Cycling_Test";
    }

    return "Unknown_Test";
}


// ============================================================
// MODULE FOLDER
// ============================================================

String getModuleFolder(
    uint16_t moduleNumber
)
{
    String folder = "/Module_";

    if (moduleNumber < 10)
    {
        folder += "00";
    }
    else if (moduleNumber < 100)
    {
        folder += "0";
    }

    folder += String(moduleNumber);

    return folder;
}


// ============================================================
// BASE FILENAME
// ============================================================

String getTestFilename(
    const String& date,
    uint16_t moduleNumber,
    TestMode mode,
    uint32_t cycleNumber
)
{
    String filename;

    filename += date;

    filename += "_Module_";

    if (moduleNumber < 10)
    {
        filename += "00";
    }
    else if (moduleNumber < 100)
    {
        filename += "0";
    }

    filename += String(moduleNumber);

    filename += "_";

    filename += testModeToFilename(mode);

    filename += "_Cycle_";

    filename += String(cycleNumber);

    filename += ".csv";

    return filename;
}


// ============================================================
// BASE COMPLETE PATH
// ============================================================

String getTestFilePath(
    const String& date,
    uint16_t moduleNumber,
    TestMode mode,
    uint32_t cycleNumber
)
{
    return
        getModuleFolder(moduleNumber)
        +
        "/"
        +
        getTestFilename(
            date,
            moduleNumber,
            mode,
            cycleNumber
        );
}

// ============================================================
// ENSURE MODULE FOLDER
// ============================================================

bool ensureModuleFolder(
    uint16_t moduleNumber
)
{
    String folder =
        getModuleFolder(moduleNumber);

    if (SD.exists(folder))
    {
        return true;
    }

    return SD.mkdir(folder);
}

// ============================================================
// FILE EXISTS
// ============================================================

bool fileExists(
    const String& path
)
{
    return SD.exists(path);
}

// ============================================================
// UNIQUE FILE PATH
// ============================================================

String getUniqueFilePath(
    const String& requestedPath
)
{
    if (!SD.exists(requestedPath))
    {
        return requestedPath;
    }


    int extensionPosition =
        requestedPath.lastIndexOf(".csv");


    if (extensionPosition < 0)
    {
        return "";
    }


    String base =
        requestedPath.substring(
            0,
            extensionPosition
        );


    String extension =
        requestedPath.substring(
            extensionPosition
        );


    for (uint16_t suffix = 1;
         suffix <= 9999;
         suffix++)
    {
        char suffixText[10];


        snprintf(
            suffixText,
            sizeof(suffixText),
            "_%02u",
            suffix
        );


        String candidate =
            base +
            String(suffixText) +
            extension;


        if (!SD.exists(candidate))
        {
            return candidate;
        }
    }


    return "";
}

// ============================================================
// VALIDATE CSV PATH
// ============================================================

bool isValidCsvPath(
    const String& path
)
{
    if (!path.startsWith("/Module_"))
    {
        return false;
    }


    if (path.indexOf("..") >= 0)
    {
        return false;
    }


    if (!path.endsWith(".csv"))
    {
        return false;
    }


    return true;
}

// ============================================================
// CSV FILE LIST JSON
// ============================================================

String getCsvFileListJson()
{
    String json = "[";


    File root =
        SD.open("/");


    if (!root)
    {
        return "[]";
    }


    if (!root.isDirectory())
    {
        root.close();

        return "[]";
    }


    bool firstEntry = true;


    while (true)
    {
        File moduleFolder =
            root.openNextFile();


        if (!moduleFolder)
        {
            break;
        }


        if (!moduleFolder.isDirectory())
        {
            moduleFolder.close();

            continue;
        }


        String modulePath =
            String(
                moduleFolder.path()
            );


        // Only Module_xxx folders.

        if (!modulePath.startsWith(
                "/Module_"))
        {
            moduleFolder.close();

            continue;
        }


        moduleFolder.rewindDirectory();


        while (true)
        {
            File file =
                moduleFolder.openNextFile();


            if (!file)
            {
                break;
            }


            if (!file.isDirectory())
            {
                String filePath =
                    String(
                        file.path()
                    );


                if (filePath.endsWith(".csv"))
                {
                    if (!firstEntry)
                    {
                        json += ",";
                    }


                    json += "{";


                    json += "\"module\":\"";
                    json += modulePath;
                    json += "\",";


                    json += "\"file\":\"";
                    json += filePath;
                    json += "\",";


                    json += "\"size\":";
                    json += String(
                        file.size()
                    );


                    json += "}";


                    firstEntry = false;
                }
            }


            file.close();
        }


        moduleFolder.close();
    }


    root.close();


    json += "]";


    return json;
}