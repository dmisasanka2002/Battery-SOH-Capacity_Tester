#include "control/test_controller.h"

#include "Config.h"
#include "storage/sd_logger.h"
#include "storage/csv_format.h"
#include "rtc/rtc_manager.h"
#include "sensors/voltage_sensor.h"
#include "sensors/ina226.h"
#include "sensors/ntc_temperature.h"
#include "control/relay_control.h"
#include "web/web_dashboard.h"

static uint16_t moduleNumber = 1;
static uint16_t cycleNumber = 1;
static TestMode selectedMode = TestMode::CapacityTest;

static String testSequence[MAX_SEQUENCE_LENGTH];
static uint8_t sequenceLength = 0;

static bool testRunning = false;
static uint8_t currentStepIndex = 0;

static uint32_t testStartMillis = 0;
static uint32_t stepStartMillis = 0;
static uint32_t lastLogMillis = 0;

static String testModeDisplayName(TestMode mode)
{
    return (mode == TestMode::CapacityTest) ? "Capacity Test" : "Cycling Test";
}

static void loadDefaultSequence()
{
    static const char* DEFAULT_SEQUENCE[] = {
        "Discharging", "Rest After Discharging",
        "Charging", "Rest After Charging",
        "Discharging", "Rest After Discharging"
    };

    sequenceLength = 6;

    for (uint8_t i = 0; i < sequenceLength; i++)
    {
        testSequence[i] = DEFAULT_SEQUENCE[i];
    }
}

void testControllerBegin()
{
    // relayControlBegin();
    loadDefaultSequence();
}

String testControllerGetCurrentMethodName()
{
    if (!testRunning) return "Idle";
    if (currentStepIndex >= sequenceLength) return "Complete";
    return testSequence[currentStepIndex];
}

bool testControllerSetSequence(const String sequence[], uint8_t length)
{
    if (testRunning || length == 0 || length > MAX_SEQUENCE_LENGTH) return false;

    sequenceLength = length;

    for (uint8_t i = 0; i < length; i++)
    {
        testSequence[i] = sequence[i];
    }

    return true;
}

void testControllerSetConfig(uint16_t newModule, uint16_t newCycle, TestMode newMode)
{
    if (testRunning) return;

    moduleNumber = newModule;
    cycleNumber = newCycle;
    selectedMode = newMode;
}

bool testControllerStart()
{
    if (testRunning) return false;
    if (sequenceLength == 0) loadDefaultSequence();

    bool opened = sdLogger.startTestFile(
        getDateString(), moduleNumber, selectedMode, cycleNumber);

    if (!opened) return false;

    testRunning = true;
    currentStepIndex = 0;

    testStartMillis = millis();
    stepStartMillis = testStartMillis;
    lastLogMillis = testStartMillis;

    // applyTestMethod(stringToTestMethod(testControllerGetCurrentMethodName()));

    return true;
}

void testControllerStop()
{
    if (!testRunning) return;

    testRunning = false;
    // applyTestMethod(TestMethod::Idle);
    sdLogger.closeFile();
}

bool testControllerIsRunning()          { return testRunning; }
uint16_t testControllerGetModuleNumber(){ return moduleNumber; }
uint16_t testControllerGetCycleNumber() { return cycleNumber; }
TestMode testControllerGetMode()        { return selectedMode; }
String testControllerGetCurrentFilePath() { return sdLogger.getCurrentFilePath(); }
uint8_t testControllerGetSequenceLength() { return sequenceLength; }

String testControllerGetSequenceStep(uint8_t index)
{
    if (index >= sequenceLength) return "";
    return testSequence[index];
}

uint32_t testControllerGetElapsedSeconds()
{
    if (!testRunning) return 0;
    return (millis() - testStartMillis) / 1000;
}

void testControllerUpdate()
{
    if (!testRunning) return;

    uint32_t now = millis();

    if (now - lastLogMillis >= LOG_INTERVAL_MS)
    {
        lastLogMillis = now;

        float voltage    = readBatteryVoltage();
        // float current    = currentSensor.readCurrentAmps();
        float batteryTemp = readBatteryTemperature();
        float envTemp     = readEnvironmentTemperature();

        DataRecord record;
        record.timestamp   = getTimestamp();
        record.moduleNumber = moduleNumber;
        record.cycleNumber  = cycleNumber;
        record.voltage      = voltage;
        // record.current      = current;
        record.batteryTemperature = batteryTemp;
        record.environmentTemperature = envTemp;
        record.mode   = testModeDisplayName(selectedMode);
        record.method = testControllerGetCurrentMethodName();

        sdLogger.logRecord(record);

        // webDashboardUpdate(voltage, current, batteryTemp, envTemp);
        webDashboardUpdate(voltage, 0, batteryTemp, envTemp);

    }

    // Fixed-duration step advance — placeholder. Replace with a
    // voltage-cutoff or capacity-threshold check for the real test.
    if (now - stepStartMillis >= METHOD_STEP_DURATION_MS)
    {
        currentStepIndex++;
        stepStartMillis = now;

        if (currentStepIndex >= sequenceLength)
        {
            testControllerStop();
            return;
        }

        // applyTestMethod(stringToTestMethod(testControllerGetCurrentMethodName()));
    }
}