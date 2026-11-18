#include "clock.h"
#include <time.h>
#include <Preferences.h>

const ClockZone CLOCK_ZONES[] = {
  {"Europe/Paris (CET)",            "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europe/London (GMT)",           "GMT0BST,M3.5.0/1,M10.5.0"},
  {"Europe/Lisbon (WET)",           "WET0WEST,M3.5.0/1,M10.5.0"},
  {"Europe/Athens (EET)",           "EET-2EEST,M3.5.0/3,M10.5.0/4"},
  {"Europe/Moscow",                 "MSK-3"},
  {"Asia/Dubai",                    "<+04>-4"},
  {"Asia/Kolkata",                  "IST-5:30"},
  {"Asia/Shanghai",                 "CST-8"},
  {"Asia/Tokyo",                    "JST-9"},
  {"Australia/Sydney",              "AEST-10AEDT,M10.1.0,M4.1.0/3"},
  {"Pacific/Auckland",              "NZST-12NZDT,M9.5.0,M4.1.0/3"},
  {"America/Sao_Paulo",             "<-03>3"},
  {"America/New_York (Eastern)",    "EST5EDT,M3.2.0,M11.1.0"},
  {"America/Chicago (Central)",     "CST6CDT,M3.2.0,M11.1.0"},
  {"America/Denver (Mountain)",     "MST7MDT,M3.2.0,M11.1.0"},
  {"America/Los_Angeles (Pacific)", "PST8PDT,M3.2.0,M11.1.0"},
  {"UTC",                           "UTC0"},
};
const int CLOCK_ZONE_COUNT = sizeof CLOCK_ZONES / sizeof CLOCK_ZONES[0];

static int zoneIdx = CLOCK_ZONE_DEFAULT;
static bool h12 = false;

static void loadSettings() {
  Preferences p;
  if (!p.begin("clock", true)) return;
  int z = p.getInt("zone", CLOCK_ZONE_DEFAULT);
  zoneIdx = (z >= 0 && z < CLOCK_ZONE_COUNT) ? z : CLOCK_ZONE_DEFAULT;
  h12 = p.getBool("h12", false);
  p.end();
}

int clockZoneIndex() { return zoneIdx; }
bool clock12h() { return h12; }

void clockSetup(int zone, bool use12h) {
  if (zone < 0 || zone >= CLOCK_ZONE_COUNT) zone = CLOCK_ZONE_DEFAULT;
  zoneIdx = zone;
  h12 = use12h;
  Preferences p;
  if (p.begin("clock", false)) { p.putInt("zone", zone); p.putBool("h12", use12h); p.end(); }
  setenv("TZ", CLOCK_ZONES[zoneIdx].posix, 1);
  tzset();
}

static bool forcedOff = false;

void clockBegin() {
  loadSettings();
  configTzTime(CLOCK_ZONES[zoneIdx].posix, "pool.ntp.org", "time.google.com");
}

void clockForceOff(bool off) { forcedOff = off; }

static bool now(struct tm& t) {
  if (forcedOff) return false;
  time_t s = time(nullptr);
  localtime_r(&s, &t);
  return t.tm_year + 1900 >= 2025;
}

bool clockValid() {
  struct tm t;
  return now(t);
}

bool clockHHMM(char out[6]) {
  struct tm t;
  if (!now(t)) return false;
  int hh = t.tm_hour;
  if (h12) { hh %= 12; if (hh == 0) hh = 12; }
  snprintf(out, 6, "%02d:%02d", hh, t.tm_min);
  return true;
}

bool clockDate(char* out, size_t n) {
  struct tm t;
  if (!now(t)) return false;
  strftime(out, n, "%A %d %B", &t);          // C locale: English names, ASCII
  if (out[0] && strlen(out) > 2) {            // "Wednesday 05 November" -> "Wednesday 5 November"
    char* d = strchr(out, ' ');
    if (d && d[1] == '0') memmove(d + 1, d + 2, strlen(d + 2) + 1);
  }
  return true;
}
