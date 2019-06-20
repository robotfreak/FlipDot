/*
 * Used libraires:
 * TimeLib https://github.com/PaulStoffregen/Time
 * RTClib https://github.com/adafruit/RTClib
 * SHT1x https://github.com/practicalarduino/SHT1x
*/

#include <Wire.h>
#include <TimeLib.h>
#include <RTClib.h>
#include <SHT1x.h>
#include "Flipdot.h"
#include "FlipdotUtils.h"


FlipDot fd(50, FD_ROWS);
FlipDotUtils fdu(fd);
int fontWidth, fontHeight;

#define dataPin  4
#define clockPin 5
SHT1x sht1x(dataPin, clockPin);

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup() {
  fd.begin();
  delay(1000);
  fdu.printString(0, 0, ON, XLARGE,"v10");
  fdu.updatePanel();
  delay(1000);
  if (! rtc.begin()) {
  }
  else if (! rtc.isrunning()) {
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

//tmElements_t tm;
//tmElements_t last_tm;
unsigned long current_time;
unsigned long last_time;
int mode = 1;
int hours, minutes;

void loop() {
  char buffer[8];
  float temp_c;
  float temp_f;
  float humidity;

  DateTime tm = rtc.now();

  current_time = millis();
  if (current_time - last_time  >= 20 * 1000) {
    last_time = millis();
    if (mode == 1)
    {
      temp_c = sht1x.readTemperatureC();
      sprintf(buffer, " %c° ", temp_c);
      fdu.printString(0, 0, ON, XLARGE,"v10");
    }
    else if (mode == 2)
    {
      humidity = sht1x.readHumidity();
      sprintf(buffer, " %c% ", humidity);
      fdu.printString(0, 0, ON, XLARGE,"v10");
    }
    else if (mode == 3)
    {
      hours = tm.hour();
      minutes = tm.minute();
      sprintf(buffer, "%2c:%2c ", hours, minutes);
      fdu.printString(0, 0, ON, XLARGE,"v10");
    }
    mode++; if (mode == 4) mode = 1;
    fdu.updatePanel();

  }
  delay(1000);
}
