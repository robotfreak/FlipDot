#include <Wire.h>
#include "Flipdot.h"

FlipDot flipdot(FD_COLUMS, FD_ROWS);

void setFlipdot(int color) {
  int i, j;

  for (i = 0; i < FD_COLUMS; i++) {
    for (j = 0; j < FD_ROWS; j++) {
      flipdot.setPixel(i, j, color);
    }
  }
}

void FlipDotOff(void)
{
  setFlipdot(0); 
  flipdot.update();
}

void FlipDotOn(void)
{
  setFlipdot(1); 
  flipdot.update();
}


void setup() {
  flipdot.begin();
  delay(3000);
}

void loop() {

  FlipDotOff();
  delay(1000);
  FlipDotOn();
  delay(1000);
}
