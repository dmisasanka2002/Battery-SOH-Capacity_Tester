#include "web/web_dashboard.h"
#include "storage/file_manager.h"
#include "control/test_controller.h"

#include <WiFi.h>
#include <WebServer.h>

#include <FS.h>
#include <SD.h>

#include "Config.h"

// ============================================================
// WEB SERVER
// ============================================================

static WebServer server(80);

// ============================================================
// LIVE DATA
// ============================================================

static float dashboardVoltage = 0.0f;

static float dashboardCurrent = 0.0f;

static float dashboardBatteryTemperature = 0.0f;

static float dashboardEnvironmentTemperature = 0.0f;

// ============================================================
// TEST CONFIGURATION
// ============================================================

static uint16_t selectedModuleNumber = 1;

static uint32_t selectedCycleNumber = 1;

static String selectedMode = "Capacity Test";

// ============================================================
// DASHBOARD HTML
// ============================================================

static const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(

<!DOCTYPE html>

<html>

<head>

<meta charset="UTF-8">

<meta
    name="viewport"
    content="width=device-width, initial-scale=1.0"
>

<title>Battery Capacity Tester</title>


<style>

* {
    box-sizing: border-box;
}

body {
    margin: 0;

    background: #f3f4f6;

    font-family:
        Arial,
        Helvetica,
        sans-serif;

    color: #1f2937;
}


.header {
    background: #1f2937;

    color: white;

    padding: 18px;

    text-align: center;
}


.header h1 {
    margin: 0;

    font-size: 22px;
}


.container {
    width: 100%;

    max-width: 1100px;

    margin: auto;

    padding: 15px;
}


.card {
    background: white;

    border-radius: 12px;

    padding: 18px;

    margin-bottom: 15px;

    box-shadow:
        0 2px 8px
        rgba(0,0,0,0.08);
}


.card h2 {
    margin-top: 0;

    font-size: 18px;
}


/* ==========================================================
   LIVE VALUES
   ========================================================== */

.status-grid {
    display: grid;

    grid-template-columns:
        repeat(auto-fit, minmax(160px, 1fr));

    gap: 12px;
}


.value-box {
    background: #f8fafc;

    border-radius: 10px;

    padding: 14px;
}


.label {
    font-size: 13px;

    color: #667085;

    margin-bottom: 5px;
}


.number {
    font-size: 24px;

    font-weight: bold;
}


/* ==========================================================
   CONTROLS
   ========================================================== */

.controls {
    display: grid;

    grid-template-columns:
        repeat(auto-fit, minmax(180px, 1fr));

    gap: 12px;
}


label {
    font-size: 14px;
}


input,
select {
    width: 100%;

    padding: 10px;

    margin-top: 5px;

    border:
        1px solid #cbd5e1;

    border-radius: 7px;

    font-size: 16px;
}


button {
    width: 100%;

    padding: 12px;

    border: none;

    border-radius: 8px;

    font-size: 16px;

    cursor: pointer;
}


.primary {
    background: #2563eb;

    color: white;
}


.danger {
    background: #dc2626;

    color: white;
}


.secondary {
    background: #e5e7eb;

    color: #111827;
}


/* ==========================================================
   PLOT
   ========================================================== */

.plot-controls {
    display: grid;

    grid-template-columns:
        repeat(auto-fit, minmax(160px, 1fr));

    gap: 10px;

    margin-bottom: 12px;
}


.plot-container {
    position: relative;

    width: 100%;

    height: 320px;

    background: white;

    border:
        1px solid #d1d5db;

    border-radius: 8px;

    overflow: hidden;
}


canvas {
    width: 100%;

    height: 100%;
}


.plot-info {
    margin-top: 8px;

    font-size: 13px;

    color: #667085;
}

/* ============================================================
   SAVED TEST FILES
   ============================================================ */

.file-row
{
    display: flex;

    align-items: center;

    justify-content: space-between;

    gap: 12px;

    padding: 12px 0;

    border-bottom: 1px solid #e5e7eb;
}


.file-row span
{
    overflow-wrap: anywhere;

    font-size: 14px;
}


.download-button
{
    width: auto;

    min-width: 100px;

    margin: 0;

    padding: 8px 14px;
}


/* ==========================================================
   MOBILE
   ========================================================== */

@media (max-width: 600px) {

    .container {
        padding: 10px;
    }

    .header h1 {
        font-size: 19px;
    }

    .number {
        font-size: 21px;
    }

    .plot-container {
        height: 260px;
    }

}

</style>

</head>


<body>


<div class="header">

<h1>
Battery Capacity Tester
</h1>

</div>


<div class="container">


<!-- ======================================================
     LIVE VALUES
     ====================================================== -->

<div class="card">

<h2>
Live Measurements
</h2>


<div class="status-grid">


<div class="value-box">

<div class="label">
Voltage
</div>

<div class="number">

<span id="voltage">
0.000
</span>

V

</div>

</div>


<div class="value-box">

<div class="label">
Current
</div>

<div class="number">

<span id="current">
0.000
</span>

A

</div>

</div>


<div class="value-box">

<div class="label">
Battery Temperature
</div>

<div class="number">

<span id="batteryTemp">
0.0
</span>

°C

</div>

</div>


<div class="value-box">

<div class="label">
Environment Temperature
</div>

<div class="number">

<span id="environmentTemp">
0.0
</span>

°C

</div>

</div>


</div>

</div>


<!-- ======================================================
     REAL-TIME PLOT
     ====================================================== -->

<div class="card">

<h2>
Real-Time Plot
</h2>


<div class="plot-controls">


<div>

<label>
Measurement
</label>

<select id="plotType">

<option value="voltage">
Voltage
</option>

<option value="current">
Current
</option>

</select>

</div>


<div>

<label>
Time Window
</label>

<select id="timeWindow">

<option value="30">
30 seconds
</option>

<option value="60" selected>
60 seconds
</option>

<option value="300">
5 minutes
</option>

</select>

</div>


</div>


<div class="plot-container">

<canvas id="plot"></canvas>

</div>


<div class="plot-info">

<span id="plotName">
Voltage
</span>

|

Latest:

<span id="plotLatest">
0.000
</span>

</div>


</div>


<!-- ======================================================
     CONFIGURATION
     ====================================================== -->

<div class="card">

<h2>
Test Configuration
</h2>


<div class="controls">


<div>

<label>
Module Number
</label>

<input
    id="moduleNumber"
    type="number"
    min="1"
    value="1"
>

</div>


<div>

<label>
Cycle Number
</label>

<input
    id="cycleNumber"
    type="number"
    min="1"
    value="1"
>

</div>


<div>

<label>
Mode
</label>

<select id="mode">

<option>
Capacity Test
</option>

<option>
Cycling Test
</option>

</select>

</div>


</div>


<br>


<button
    class="secondary"
    onclick="saveConfiguration()"
>
Save Configuration
</button>


</div>


<!-- ======================================================
     TEST CONTROL
     ====================================================== -->

<div class="card">

<h2>
Test Control
</h2>


<button
    class="primary"
    onclick="startTest()"
>
Start Test
</button>


<br>
<br>


<button
    class="danger"
    onclick="stopTest()"
>
Stop Test
</button>


<p id="testMessage">
System idle.
</p>


</div>

<!-- =====================================================
     SAVED TEST FILES
     ===================================================== -->

<div class="card">

    <h2>Saved Test Files</h2>

    <div id="fileList">
        Loading files...
    </div>

</div>

</div>


<script>


// ==========================================================
// PLOT DATA
// ==========================================================

const voltageData = [];

const currentData = [];


// Time values in milliseconds.

const timeData = [];


// ==========================================================
// CANVAS
// ==========================================================

const canvas =
    document.getElementById("plot");

const ctx =
    canvas.getContext("2d");


// ==========================================================
// RESIZE CANVAS
// ==========================================================

function resizeCanvas()
{
    const rect =
        canvas.getBoundingClientRect();


    canvas.width =
        rect.width *
        window.devicePixelRatio;


    canvas.height =
        rect.height *
        window.devicePixelRatio;


    ctx.scale(
        window.devicePixelRatio,
        window.devicePixelRatio
    );
}


window.addEventListener(
    "resize",
    resizeCanvas
);


resizeCanvas();


// ==========================================================
// ADD MEASUREMENT
// ==========================================================

function addMeasurement(data)
{
    const now =
        Date.now();


    timeData.push(now);

    voltageData.push(
        Number(data.voltage)
    );

    currentData.push(
        Number(data.current)
    );


    removeOldData(now);
}


// ==========================================================
// REMOVE OLD DATA
// ==========================================================

function removeOldData(now)
{
    const windowSeconds =
        Number(
            document.getElementById(
                "timeWindow"
            ).value
        );


    const minimumTime =
        now -
        windowSeconds * 1000;


    while (
        timeData.length > 0 &&
        timeData[0] < minimumTime
    )
    {
        timeData.shift();

        voltageData.shift();

        currentData.shift();
    }
}


// ==========================================================
// DRAW PLOT
// ==========================================================

function drawPlot()
{
    const rect =
        canvas.getBoundingClientRect();


    const width =
        rect.width;


    const height =
        rect.height;


    ctx.clearRect(
        0,
        0,
        width,
        height
    );


    const plotType =
        document.getElementById(
            "plotType"
        ).value;


    const values =
        plotType === "voltage"
        ? voltageData
        : currentData;


    if (values.length < 2)
    {
        return;
    }


    // ------------------------------------------------------
    // Find range
    // ------------------------------------------------------

    let minValue =
        Math.min(...values);


    let maxValue =
        Math.max(...values);


    if (
        maxValue === minValue
    )
    {
        maxValue += 1;

        minValue -= 1;
    }


    const marginLeft = 50;

    const marginRight = 15;

    const marginTop = 15;

    const marginBottom = 30;


    const plotWidth =
        width -
        marginLeft -
        marginRight;


    const plotHeight =
        height -
        marginTop -
        marginBottom;


    // ------------------------------------------------------
    // Grid
    // ------------------------------------------------------

    ctx.strokeStyle =
        "#e5e7eb";

    ctx.lineWidth = 1;


    for (
        let i = 0;
        i <= 5;
        i++
    )
    {
        const y =
            marginTop +
            (plotHeight * i / 5);


        ctx.beginPath();

        ctx.moveTo(
            marginLeft,
            y
        );

        ctx.lineTo(
            width - marginRight,
            y
        );

        ctx.stroke();


        const value =
            maxValue -
            (
                (maxValue - minValue)
                * i / 5
            );


        ctx.fillStyle =
            "#667085";

        ctx.font =
            "11px Arial";

        ctx.fillText(
            value.toFixed(2),
            5,
            y + 4
        );
    }


    // ------------------------------------------------------
    // Plot line
    // ------------------------------------------------------

    ctx.strokeStyle =
        "#2563eb";

    ctx.lineWidth = 2;

    ctx.beginPath();


    for (
        let i = 0;
        i < values.length;
        i++
    )
    {
        const x =
            marginLeft +
            (
                i /
                (values.length - 1)
            ) *
            plotWidth;


        const y =
            marginTop +
            (
                1 -
                (
                    (values[i] - minValue) /
                    (maxValue - minValue)
                )
            ) *
            plotHeight;


        if (i === 0)
        {
            ctx.moveTo(
                x,
                y
            );
        }
        else
        {
            ctx.lineTo(
                x,
                y
            );
        }
    }


    ctx.stroke();
}


// ==========================================================
// UPDATE NUMERICAL DATA
// ==========================================================

async function updateStatus()
{
    try
    {
        const response =
            await fetch(
                "/api/status"
            );


        const data =
            await response.json();


        document.getElementById(
            "voltage"
        ).textContent =
            Number(
                data.voltage
            ).toFixed(3);


        document.getElementById(
            "current"
        ).textContent =
            Number(
                data.current
            ).toFixed(3);


        document.getElementById(
            "batteryTemp"
        ).textContent =
            Number(
                data.batteryTemperature
            ).toFixed(1);


        document.getElementById(
            "environmentTemp"
        ).textContent =
            Number(
                data.environmentTemperature
            ).toFixed(1);


        addMeasurement(data);

        drawPlot();


        const plotType =
            document.getElementById(
                "plotType"
            ).value;


        const latest =
            plotType === "voltage"
            ? data.voltage
            : data.current;


        document.getElementById(
            "plotLatest"
        ).textContent =
            Number(
                latest
            ).toFixed(3);


        document.getElementById(
            "plotName"
        ).textContent =
            plotType === "voltage"
            ? "Voltage (V)"
            : "Current (A)";
    }

    catch(error)
    {
        console.log(error);
    }
}


// ==========================================================
// CONFIGURATION
// ==========================================================

async function saveConfiguration()
{
    const moduleNumber =
        document.getElementById(
            "moduleNumber"
        ).value;


    const cycleNumber =
        document.getElementById(
            "cycleNumber"
        ).value;


    const mode =
        document.getElementById(
            "mode"
        ).value;


    const url =
        "/api/config"
        + "?module="
        + encodeURIComponent(
            moduleNumber
        )
        + "&cycle="
        + encodeURIComponent(
            cycleNumber
        )
        + "&mode="
        + encodeURIComponent(
            mode
        );


    try
    {
        const response =
            await fetch(url);


        const text =
            await response.text();


        alert(text);
    }

    catch(error)
    {
        alert(
            "Configuration update failed."
        );
    }
}


// ==========================================================
// START TEST
// ==========================================================

async function startTest()
{
    const response =
        await fetch(
            "/api/test/start"
        );


    const text =
        await response.text();


    document.getElementById(
        "testMessage"
    ).textContent =
        text;
}


// ==========================================================
// STOP TEST
// ==========================================================

async function stopTest()
{
    const response =
        await fetch(
            "/api/test/stop"
        );


    const text =
        await response.text();


    document.getElementById(
        "testMessage"
    ).textContent =
        text;
}


// ==========================================================
// PLOT SELECTION
// ==========================================================

document.getElementById(
    "plotType"
).addEventListener(
    "change",
    drawPlot
);


document.getElementById(
    "timeWindow"
).addEventListener(
    "change",
    function()
    {
        removeOldData(
            Date.now()
        );

        drawPlot();
    }
);

// ============================================================
// SAVED TEST FILES
// ============================================================

async function loadFiles()
{
    try
    {
        const response =
            await fetch("/api/files");


        if (!response.ok)
        {
            throw new Error(
                "File list request failed"
            );
        }


        const files =
            await response.json();


        const container =
            document.getElementById(
                "fileList"
            );


        container.innerHTML = "";


        if (files.length === 0)
        {
            container.innerHTML =
                "<p>No CSV files found.</p>";

            return;
        }


        files.forEach(
            function(file)
            {
                const row =
                    document.createElement(
                        "div"
                    );


                row.className =
                    "file-row";


                const name =
                    document.createElement(
                        "span"
                    );

                const separator =
                    file.file.lastIndexOf("/");

                const filename =
                    file.file.substring(
                        separator + 1
                    );

                name.textContent =
                    filename;


                const button =
                    document.createElement(
                        "button"
                    );


                button.textContent =
                    "Download";


                button.className =
                    "download-button";


                button.onclick =
                    function()
                    {
                        window.location.href =
                            "/download?file=" +
                            encodeURIComponent(
                                file.file
                            );
                    };


                row.appendChild(name);

                row.appendChild(button);

                container.appendChild(row);
            }
        );
    }
    catch(error)
    {
        console.error(
            "File loading error:",
            error
        );


        document.getElementById(
            "fileList"
        ).innerHTML =
            "<p>Unable to load saved files.</p>";
    }
}


// ==========================================================
// START DASHBOARD UPDATES
// ==========================================================

setInterval(
    updateStatus,
    500
);

loadFiles();

updateStatus();

</script>


</body>

</html>

)rawliteral";

// ============================================================
// ROOT
// ============================================================

static void handleRoot()
{
    server.send(
        200,
        "text/html",
        DASHBOARD_HTML);
}

// ============================================================
// STATUS API
// ============================================================

static void handleStatus()
{
    String json = "{";
    json += "\"status\":\"" + String(testControllerIsRunning() ? "RUNNING" : "IDLE") + "\",";
    json += "\"voltage\":" + String(dashboardVoltage, 6) + ",";
    json += "\"current\":" + String(dashboardCurrent, 6) + ",";
    json += "\"batteryTemperature\":" + String(dashboardBatteryTemperature, 3) + ",";
    json += "\"environmentTemperature\":" + String(dashboardEnvironmentTemperature, 3) + ",";
    json += "\"module\":" + String(testControllerGetModuleNumber()) + ",";
    json += "\"cycle\":" + String(testControllerGetCycleNumber()) + ",";
    json += "\"mode\":\"" + testModeDisplayName(testControllerGetMode()) + "\",";
    json += "\"method\":\"" + testControllerGetCurrentMethodName() + "\",";
    json += "\"elapsed\":" + String(testControllerGetElapsedSeconds()) + ",";
    json += "\"file\":\"" + testControllerGetCurrentFilePath() + "\"";
    json += "}";

    server.send(200, "application/json", json);
}

// ============================================================
// CONFIGURATION API
// ============================================================

static TestMode parseTestMode(const String& text)
{
    return (text == "Cycling Test") ? TestMode::CyclingTest : TestMode::CapacityTest;
}

static void handleConfig()
{
    bool hasUpdate = server.hasArg("module") || server.hasArg("cycle") || server.hasArg("mode");

    if (hasUpdate)
    {
        if (testControllerIsRunning())
        {
            server.send(409, "text/plain", "Configuration locked while test is running.");
            return;
        }

        uint16_t module = server.hasArg("module") ? server.arg("module").toInt() : testControllerGetModuleNumber();
        uint16_t cycle  = server.hasArg("cycle")  ? server.arg("cycle").toInt()  : testControllerGetCycleNumber();
        TestMode mode   = server.hasArg("mode")   ? parseTestMode(server.arg("mode")) : testControllerGetMode();

        testControllerSetConfig(module, cycle, mode);

        server.send(200, "text/plain", "Configuration saved.");
        return;
    }

    String json = "{";
    json += "\"module\":" + String(testControllerGetModuleNumber()) + ",";
    json += "\"cycle\":" + String(testControllerGetCycleNumber()) + ",";
    json += "\"mode\":\"" + testModeDisplayName(testControllerGetMode()) + "\",";
    json += "\"sequence\":[";

    for (uint8_t i = 0; i < testControllerGetSequenceLength(); i++)
    {
        json += "\"" + testControllerGetSequenceStep(i) + "\"";
        if (i < testControllerGetSequenceLength() - 1) json += ",";
    }

    json += "]}";

    server.send(200, "application/json", json);
}

static void handleSequence()
{
    if (testControllerIsRunning())
    {
        server.send(409, "text/plain", "Sequence locked while test is running.");
        return;
    }

    if (!server.hasArg("sequence"))
    {
        server.send(400, "text/plain", "Missing sequence.");
        return;
    }

    String sequenceString = server.arg("sequence");
    String parsed[MAX_SEQUENCE_LENGTH];
    uint8_t count = 0;
    int start = 0;

    while (start < (int)sequenceString.length() && count < MAX_SEQUENCE_LENGTH)
    {
        int separator = sequenceString.indexOf('|', start);
        if (separator == -1)
        {
            parsed[count++] = sequenceString.substring(start);
            break;
        }
        parsed[count++] = sequenceString.substring(start, separator);
        start = separator + 1;
    }

    if (count == 0 || !testControllerSetSequence(parsed, count))
    {
        server.send(400, "text/plain", "Invalid sequence.");
        return;
    }

    server.send(200, "text/plain", "Sequence saved.");
}

// ============================================================
// START TEST
// ============================================================

static void handleStartTest()
{
    if (testControllerIsRunning())
    {
        server.send(409, "text/plain", "Test is already running.");
        return;
    }

    bool started = testControllerStart();

    server.send(started ? 200 : 500, "text/plain",
                started ? "Test started." : "Could not start test. Check SD card.");
}

// ============================================================
// STOP TEST
// ============================================================

static void handleStopTest()
{
    testControllerStop();
    server.send(200, "text/plain", "Test stopped.");
}

// ============================================================
// FILE DOWNLOAD
// ============================================================

static void handleFileDownload()
{
    if (!server.hasArg("file"))
    {
        server.send(
            400,
            "text/plain",
            "File path missing.");

        return;
    }

    String path =
        server.arg("file");

    // --------------------------------------------------------
    // Basic path protection.
    //
    // Only allow paths inside /Module_xxx/
    // --------------------------------------------------------

    if (!isValidCsvPath(path))
    {
        server.send(
            403,
            "text/plain",
            "Invalid CSV file path."
        );

        return;
    }

    // Do not allow parent-directory traversal.

    if (path.indexOf("..") >= 0)
    {
        server.send(
            403,
            "text/plain",
            "Invalid file path.");

        return;
    }

    // Only CSV files.

    if (!path.endsWith(".csv"))
    {
        server.send(
            403,
            "text/plain",
            "Only CSV files can be downloaded.");

        return;
    }

    // --------------------------------------------------------
    // Check existence.
    // --------------------------------------------------------

    if (!SD.exists(path))
    {
        server.send(
            404,
            "text/plain",
            "File not found.");

        return;
    }

    // --------------------------------------------------------
    // Open.
    // --------------------------------------------------------

    File file =
        SD.open(
            path,
            FILE_READ);

    if (!file)
    {
        server.send(
            500,
            "text/plain",
            "Could not open file.");

        return;
    }

    // --------------------------------------------------------
    // Extract filename.
    // --------------------------------------------------------

    int slashPosition =
        path.lastIndexOf('/');

    String filename =
        path.substring(
            slashPosition + 1);

    // --------------------------------------------------------
    // Tell browser to download.
    // --------------------------------------------------------

    server.sendHeader(
        "Content-Disposition",
        "attachment; filename=\"" +
            filename +
            "\"");

    // --------------------------------------------------------
    // Stream file.
    // --------------------------------------------------------

    size_t sent =
        server.streamFile(
            file,
            "text/csv");

    if (sent != file.size())
    {
        Serial.println(
            "WARNING: File transfer incomplete.");
    }

    file.close();
}

// ============================================================
// FILE LIST API
// ============================================================

static void handleFileList()
{
    String json =
        getCsvFileListJson();


    server.send(
        200,
        "application/json",
        json
    );
}

// ============================================================
// BEGIN
// ============================================================

void webDashboardBegin()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD);

    Serial.print("Dashboard IP: http://");
    Serial.println(WiFi.softAPIP());

    server.on("/", HTTP_GET, handleRoot);

    server.on("/api/status", HTTP_GET, handleStatus);

    server.on("/api/config", HTTP_GET, handleConfig);

    server.on("/api/test/start", HTTP_GET, handleStartTest);

    server.on("/api/test/stop", HTTP_GET, handleStopTest);

    server.on("/api/files", HTTP_GET, handleFileList);

    server.on("/download", HTTP_GET, handleFileDownload);

    server.on("/api/sequence", HTTP_GET, handleSequence);

    server.onNotFound(
        []()
        {
            server.send(
                404,
                "text/plain",
                "Page not found.");
        });

    server.begin();
}

// ============================================================
// HANDLE REQUESTS
// ============================================================

void webDashboardHandle()
{
    server.handleClient();
}

// ============================================================
// UPDATE LIVE VALUES
// ============================================================

void webDashboardUpdate(
    float voltage,
    float current,
    float batteryTemperature,
    float environmentTemperature)
{
    dashboardVoltage =
        voltage;

    dashboardCurrent =
        current;

    dashboardBatteryTemperature =
        batteryTemperature;

    dashboardEnvironmentTemperature =
        environmentTemperature;
}