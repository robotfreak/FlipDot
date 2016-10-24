#include <Wire.h>
#include "FlipDot.h"

#define LATCH 10
#define OE 9

FlipDot fd(LATCH, OE);

void FlipDotOff(void)
{
  memset(fd.fdMtx, 0x00, sizeof(fd.fdMtx));
  fd.Update();
}

void FlipDotOn(void)
{
  memset(fd.fdMtx, 0xFF, sizeof(fd.fdMtx));
  fd.Update();
}


void setup() {
  delay(3000);
}

void loop() {

  FlipDotOff();
//  delay(1000);
  FlipDotOn();
//  delay(1000);
}



