#include <Wire.h>
#include <TimeLib.h>
#include <RTClib.h>
#include <SHT1x.h>
#include <SPI.h>
#include "FlipDot.h"

#define LATCH 10
#define OE 9


FlipDot flipdot(50, FD_ROWS);


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
  flipdot.setTextSize(1);
  flipdot.setCursor(0,0);
  for (char c = '0'; c <= '7'; c++) {
    flipdot.write(c);
  } 
  flipdot.setCursor(0,8);
  for (char c = 'A'; c <= 'H'; c++) {
    flipdot.write(c);
  } 
  flipdot.update();
  delay(1000);

  flipdot.setCursor(0,0);
  flipdot.setTextSize(2);
  for (char c = 'A'; c <= 'D'; c++) {
    flipdot.write(c);
  } 
  flipdot.update();
  delay(1000);

  flipdot.setCursor(0,0);
  flipdot.setTextSize(2);
  for (char c = '0'; c <= '3'; c++) {
    flipdot.write(c);
  } 
  flipdot.update();
  delay(1000);
}

