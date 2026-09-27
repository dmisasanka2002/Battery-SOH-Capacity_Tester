#include "storage/csv_format.h"


// ============================================================
// CSV HEADER
// ============================================================

String getCsvHeader()
{
    return
        "Timestamp,"
        "Elapsed_ms,"
        "Module Nb.,"
        "Voltage,"
        "Current,"
        "Battery Temperature,"
        "Environment Temperature,"
        "Mode,"
        "Method,"
        "Cycle Nb.";
}


// ============================================================
// CSV ROW
// ============================================================

String getCsvRow(
    const DataRecord& record
)
{
    String row;

    row.reserve(180);


    row += record.timestamp;
    row += ",";


    row += String(
        record.elapsedMs
    );
    row += ",";


    row += String(
        record.moduleNumber
    );
    row += ",";


    row += String(
        record.voltage,
        6
    );
    row += ",";


    row += String(
        record.current,
        6
    );
    row += ",";


    row += String(
        record.batteryTemperature,
        3
    );
    row += ",";


    row += String(
        record.environmentTemperature,
        3
    );
    row += ",";


    row += record.mode;
    row += ",";


    row += record.method;
    row += ",";


    row += String(
        record.cycleNumber
    );


    return row;
}