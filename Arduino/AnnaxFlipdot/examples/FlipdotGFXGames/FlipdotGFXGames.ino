// Button Pin Setup
const int l_up_button = 3; //Left player up button pin
const int l_down_button = 6; //Left player down button pin
const int r_up_button = 7; //Right player up button pin
const int r_down_button = 8; //Right player down button pin
const int buttonRotate = 3; // Rotate
const int buttonRight = 6;  // Right
const int buttonLeft = 7;   // Left
const int buttonDown = 8;   // Down

int gameMode = 0;

//Screen Size Parameters
#define NUMROWS 16
#define NUMCOLS 50

int x_pixels = NUMCOLS;
int y_pixels = NUMROWS;


#include <Adafruit_GFX.h>
#include "Flipdot.h"
#include "FlipdotUtils.h"

FlipDot flipdot(50, FD_ROWS);
FlipDotUtils fdu(flipdot);

void setup() {
  int gidx;
  pinMode(l_up_button, INPUT_PULLUP);
  pinMode(l_down_button, INPUT_PULLUP);
  pinMode(r_up_button, INPUT_PULLUP);
  pinMode(r_down_button, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Arduino GFX Games");


  flipdot.begin();
  fdu.setSerialDebug(false);
  flipdot.setPanels(2);
  flipdot.setOnTime(1200);

  // Clear the buffer.
  flipdot.fillScreen(BLACK);;
  flipdot.update();

  // wait for keypress 
  while(gameMode == 0) {
    gameMode = readBut();
    if (Serial.available()) {
      int ch = Serial.read();
      gameMode = ch -'0';
    }
  }
  Serial.println(gameMode);

  switch (gameMode) {
      case 1:
        golSetup();
      break;
      case 2:
        pongSetup();
      break;
      case 3:
        tetrisSetup();
      break;
      default:
      break;
  }


}

void loop() {
  switch (gameMode) {
      case 1:
        golLoop();
      break;
      case 2:
        pongLoop();
      break;
      case 3:
        tetrisLoop();
      break;
      default:
      break;
  }

}
