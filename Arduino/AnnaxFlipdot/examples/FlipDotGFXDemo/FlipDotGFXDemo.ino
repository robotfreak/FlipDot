#include "Flipdot.h"


FlipDot matrix(50, FD_ROWS);


void setup() {
  //Serial.begin (115200);
  //Serial.println ("FlipDot Test v1.0");
  matrix.begin();
  matrix.fillScreen(FLIPDOT_YELLOW);
  matrix.update();
  delay(1000);
  matrix.fillScreen(FLIPDOT_BLACK);
  matrix.update();
  delay(1000);
}

static const uint8_t PROGMEM
smile_bmp[] =
{ B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
},
neutral_bmp[] =
{ B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10111101,
  B10000001,
  B01000010,
  B00111100
},
frown_bmp[] =
{ B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10011001,
  B10100101,
  B01000010,
  B00111100
};

void loop() {

  for (int8_t x = 0; x <= 40; x += 8) {
    matrix.fillScreen(FLIPDOT_BLACK);
    matrix.drawBitmap(x, 0, smile_bmp, 8, 8, FLIPDOT_YELLOW);
    matrix.update();
    delay(500);

    //matrix.fillScreen(FLIPDOT_BLACK);
    //matrix.drawBitmap(x, 8, neutral_bmp, 8, 8, FLIPDOT_YELLOW);
    //matrix.update();
    //delay(500);

    matrix.fillScreen(FLIPDOT_BLACK);
    matrix.drawBitmap(x, 8, frown_bmp, 8, 8, FLIPDOT_YELLOW);
    matrix.update();
    delay(500);
  }


  matrix.drawPixel(0, 0, FLIPDOT_YELLOW);
  matrix.update();  // write the changes we just made to the display
  delay(500);

  matrix.fillScreen(FLIPDOT_BLACK);
  for (int8_t x = 0; x <= 48; x += 8) {
    matrix.drawLine(0, 0, x, 15, FLIPDOT_YELLOW);
    matrix.update();  // write the changes we just made to the display
    delay(500);
  }

  matrix.fillScreen(FLIPDOT_BLACK);
  matrix.drawRect(0, 0, 50, 16, FLIPDOT_YELLOW);
  matrix.drawRect(2, 2, 46, 12, FLIPDOT_YELLOW);
  matrix.drawRect(4, 4, 42, 8, FLIPDOT_YELLOW);
  matrix.drawRect(6, 6, 38, 4, FLIPDOT_YELLOW);
  matrix.update();  // write the changes we just made to the display
  delay(500);
  /*
    matrix.fillScreen(FLIPDOT_BLACK);
    matrix.drawCircle(25,8, 3, FLIPDOT_YELLOW);
    matrix.drawCircle(25,8, 5, FLIPDOT_YELLOW);
    matrix.drawCircle(25,8, 7, FLIPDOT_YELLOW);
    matrix.drawCircle(25,8, 9, FLIPDOT_YELLOW);
    matrix.drawCircle(25,8, 11, FLIPDOT_YELLOW);
    matrix.drawCircle(25,8, 13, FLIPDOT_YELLOW);
    matrix.drawCircle(25,8, 15, FLIPDOT_YELLOW);
    matrix.update();  // write the changes we just made to the display
    delay(500);
  */
  matrix.fillScreen(FLIPDOT_BLACK);
  matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(FLIPDOT_YELLOW);
  for (int8_t x = 0; x >= -64; x--) {
    matrix.fillScreen(FLIPDOT_BLACK);
    matrix.setCursor(x, 4);
    matrix.print("Hello World");
    matrix.update();
    delay(100);
  }
  /*
    matrix.setTextSize(1);
    matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
    matrix.setTextColor(FLIPDOT_YELLOW);
    matrix.setRotation(1);
    for (int8_t x=25; x>=-36; x--) {
      matrix.fillScreen(FLIPDOT_BLACK);
      matrix.setCursor(x,0);
      matrix.print("World");
      matrix.update();
      delay(100);
    }
    matrix.setRotation(0);
  */
}
