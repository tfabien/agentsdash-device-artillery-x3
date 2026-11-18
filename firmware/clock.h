// CORE - wall clock from NTP. Timezone and 12/24 h format are chosen on /config and kept in flash
// (namespace "clock", default Europe/Paris, 24 h). No screen, no network setup.
#pragma once
#include <Arduino.h>

struct ClockZone { const char* name; const char* posix; };
extern const ClockZone CLOCK_ZONES[];
extern const int CLOCK_ZONE_COUNT;
const int CLOCK_ZONE_DEFAULT = 0;           // Europe/Paris

int  clockZoneIndex();                      // saved zone (index in CLOCK_ZONES)
bool clock12h();                            // saved format: true = 12 h
void clockSetup(int zone, bool h12);        // saves to flash and applies at once

void clockBegin();                          // after Wi-Fi connects: starts SNTP
bool clockValid();                          // time known (year >= 2025) and not forced off
bool clockHHMM(char out[6]);                // "14:32" (24 h) or "02:32" (12 h, no AM/PM); false while unknown
bool clockDate(char* out, size_t n);        // "Wednesday 11 November"; false while unknown
void clockForceOff(bool off);               // tests (/debug/config clock_off): behave as if the time were unknown
