#include <Wire.h>
#include <TimeLib.h>
#include <RTClib.h>
#include <SHT1x.h>
#include <SPI.h>
#include "FlipDot.h"

#define LATCH 10
#define OE 9


FlipDot flipdot(FD_COLUMS, FD_ROWS);


void setup() {
  //Serial.begin (115200);
  //Serial.println ("FlipDot Test v1.0");
  flipdot.begin(LATCH, OE);
  flipdot.fillScreen(FLIPDOT_YELLOW);
  flipdot.update();
  delay(1000);
  flipdot.fillScreen(FLIPDOT_BLACK);
  flipdot.update();
  delay(1000); 
}

void loop() {
  flipdot.setTextColor(FLIPDOT_YELLOW, FLIPDOT_BLACK);
  for (char c = '0'; c <= 'z'; c++) {
    flipdot.setCursor(0,0);
    flipdot.write(c);
    flipdot.update();
    delay(400);
  } 

}

