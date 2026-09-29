#include "rtc/rtc_manager.h"

#include <Wire.h>
#include <RTClib.h>

static RTC_DS1307 rtc;
static bool rtcAvailable = false;

bool rtcBegin()
{
    rtcAvailable = rtc.begin();

    if (!rtcAvailable)
    {
        Serial.println("ERROR: RTC not detected.");
    }
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Uncomment this line to set the RTC to the date & time this sketch was compiled

    return rtcAvailable;
}

String getDateString()
{
    if (!rtcAvailable) return "0000-00-00";

    DateTime now = rtc.now();
    char buffer[11];

    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d",
             now.year(), now.month(), now.day());

    return String(buffer);
}

String getTimestamp()
{
    if (!rtcAvailable) return "0000-00-00 00:00:00.000";

    DateTime now = rtc.now();
    char buffer[24];

    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d.%03lu",
             now.year(), now.month(), now.day(),
             now.hour(), now.minute(), now.second(),
             millis() % 1000);

    return String(buffer);
}