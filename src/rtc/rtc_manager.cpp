#include "rtc/rtc_manager.h"

#include <Wire.h>
#include <RTClib.h>

static RTC_DS1307 rtc;
static bool rtcAvailable = false;

static uint32_t baseEpoch = 0;
static uint32_t baseMillis = 0;

bool rtcBegin()
{
    rtcAvailable = rtc.begin();

    if (!rtcAvailable)
    {
        Serial.println("ERROR: RTC not detected.");
        return false;
    }

    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));   // keep your one-off set line here

    if (!rtc.isrunning())
    {
        Serial.println("WARNING: RTC is not running. Set the time first.");
    }

    // Wait for the next RTC second tick, then anchor to millis().
    uint32_t first = rtc.now().unixtime();
    uint32_t waitStart = millis();

    while (rtc.now().unixtime() == first && millis() - waitStart < 1500)
    {
        delay(2);
    }

    baseEpoch = rtc.now().unixtime();
    baseMillis = millis();

    return true;
}

String getDateString()
{
    if (!rtcAvailable) return "0000-00-00";

    DateTime now(baseEpoch + (millis() - baseMillis) / 1000);
    char buffer[11];

    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d",
             now.year(), now.month(), now.day());

    return String(buffer);
}

String getTimestamp()
{
    if (!rtcAvailable) return "0000-00-00 00:00:00.000";

    uint32_t elapsed = millis() - baseMillis;
    DateTime now(baseEpoch + elapsed / 1000);
    char buffer[24];

    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d.%03lu",
             now.year(), now.month(), now.day(),
             now.hour(), now.minute(), now.second(),
             static_cast<unsigned long>(elapsed % 1000));

    return String(buffer);
}