#include <Wire.h>
#include "FlipDot.h"
#include "annax9x16.h"

#define LATCH 10
#define OE 9

FlipDot fd(LATCH, OE);
int fontWidth, fontHeight;

#define dataPin  4
#define clockPin 5

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



void setup() {
  char c = '0';

  Serial.begin(38400);
  while(!Serial);

  Serial.println("Flipdot Serial v0.1");
  initFlipDot();
  delay(3000);
  for(int i=0; i<5; i++)
  {
    fdWrite(c+i, i);
  }
  fd.Update();
  delay(3000);

  for(int i=0; i<=9; i++)
  {
    fdWrite(c+i, 0);
    fd.UpdatePanel(0);
    delay(1000);
  }
 
}

void loop() {
}


