#include <simpleDSTadjust.h>

const int8_t LATCH_PIN = 5;
const int8_t OE_PIN = 26;
const int8_t COMM_PIN = 35;
const int8_t COL_PIN = 34;

const byte LED_RED = 27;
const byte LED_GREEN = 25;
const byte LED_BLUE = 32;  

const byte STOP_PIN = 33;

// WiFi Setup
#define WIFI_SSID "ssid"
#define WIFI_PASS "password"
#define WIFI_HOSTNAME "hostname"

// change for different ntp (time servers)
#define NTP_SERVERS "0.de.pool.ntp.org", "1.de.pool.ntp.org", "2.de.pool.ntp.org"

// Change for 12 Hour/ 24 hour style clock
bool IS_STYLE_12HR = false;

#define UTC_OFFSET +1
struct dstRule StartRule = {"CEST", Last, Sun, Mar, 2, 3600}; // Central European Summer Time = UTC/GMT +2 hours
struct dstRule EndRule = {"CET", Last, Sun, Oct, 2, 0};       // Central European Time = UTC/GMT +1 hour
