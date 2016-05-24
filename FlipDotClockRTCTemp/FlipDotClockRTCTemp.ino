#include <Wire.h>
#include <TimeLib.h>
#include <RTClib.h>
#include <SHT1x.h>
#include "FlipDot.h"
//#include "6x8_vertikal_MSB_1.h"
//#include "8x14_vertikal_MSB_2.h"
#include "annax9x16.h"

#define LATCH 10
#define OE 9

FlipDot fd(LATCH, OE);
int fontWidth, fontHeight;

#define dataPin  4
#define clockPin 5
SHT1x sht1x(dataPin, clockPin);

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
void initFlipDot(void)
{
  memset(fd.fdMtx, 0x00, sizeof(fd.fdMtx));
  fontWidth = FONT_WIDTH;
  fontHeight = FONT_HEIGHT;
  fd.Update();
}

void fdWrite(int character, int offset)
{
  const unsigned char *pFtL;
  const unsigned char *pFtH;
  int col, ofs;

  ofs = character;
  //  if (ofs >=64) ofs +=32;

  pFtL = &annax9x16[(ofs - 0x20) * FONT_WIDTH];
  pFtH = &annax9x16[(ofs - 0x20) * FONT_WIDTH + FONT_WIDTH * 32];
  col = offset * (fontWidth + 1);
  for (int i = 0; i < FONT_WIDTH; i++, col++)
  {
    fd.fdMtx[1][col] = *pFtL++;
    fd.fdMtx[0][col] = *pFtH++;
  }
  col += 1;
}

void fdPrintDigit(int number, int offset)
{
  if (number >= 0 && number < 10)
  {
    fdWrite('0', offset);
    fdWrite(number + 0x30, offset + 1);
  }
  else
  {
    fdWrite((number / 10) + 0x30, offset);
    fdWrite((number % 10) + 0x30, offset + 1);
  }
}


void setup() {
  initFlipDot();
  delay(1000);
  fdWrite('v', 1);
  fdWrite('1', 2);
  fdWrite('0', 3);
  fd.Update();
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
      fdWrite(' ', 0);
      fdPrintDigit((int) temp_c, 1);
      fdWrite(0x22, 3);
      fdWrite(' ', 4);
    }
    else if (mode == 2)
    {
      humidity = sht1x.readHumidity();
      fdWrite(' ', 0);
      fdPrintDigit((int) humidity, 1);
      fdWrite('%', 3);
      fdWrite(' ', 4);
    }
    else if (mode == 3)
    {
      hours = tm.hour();
      minutes = tm.minute();
      
      fdPrintDigit(hours, 0);
      fdWrite(':', 2);
      fdPrintDigit(minutes, 3);
    }
    mode++; if (mode == 4) mode = 1;
    fd.Update();

  }
  delay(1000);
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}


