#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <SPI.h>

#include "Flipdot.h"

FlipDot::FlipDot(int _sizeX, int _sizeY)
{
  this->sizeX = _sizeX;
  this->sizeY = _sizeY;
//  this->invert = invert; 
}

void FlipDot::begin()
{
  this->latchPin = LATCH;
  this->oePin = OE;
  this->resPin = RES;
  this->colPin = COL;

  memset(fdMtx, 0x00, sizeof(fdMtx));

  SPI.begin ();
  pinMode(this->latchPin, OUTPUT);
  pinMode(this->oePin, OUTPUT);
  pinMode(this->resPin, OUTPUT);
  pinMode(this->colPin, OUTPUT);
  digitalWrite (this->resPin, HIGH);
  delayMicroseconds(100);
  bitSet  (this->fdCtrl, FD_FRAME_SIG);
  ShiftOut();
  digitalWrite (this->oePin, LOW);
  digitalWrite (this->colPin, HIGH);
}

// Reverse the order of bits in a byte.
// I.e. MSB is swapped with LSB, etc.
byte FlipDot::bitReverse( byte x )
{
   x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
   x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
   x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
   return x;    
} 

void FlipDot::setPixel(int16_t x, int16_t y, uint16_t color)
{
//  int y2dot[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
  int idx, dot;
  if (y > 7) 
  {
    idx = 1;
    dot = (1 << (y-8));
//    dot = y2dot[y-8];
  }
  else 
  {
    idx = 0;
    dot = (1 << y);
//    dot = y2dot[y];
  }
  if (color)
    this->fdMtx[idx][x] |= dot;
  else
    this->fdMtx[idx][x] &= ~dot;
}

void FlipDot::ShiftOut(void)
{
  digitalWrite (this->latchPin, LOW);
  SPI.transfer (~(this->fdCtrl));
  SPI.transfer (this->fdRow2);
  SPI.transfer (this->fdRow1);
  digitalWrite (this->latchPin, HIGH);
}


void FlipDot::updatePanel(int panel)
{
  int f = 0;
  int col = 0;

  if (panel < 0 || panel >= FD_PANELS)
    return;

  for (f = 0; f < panel; f++)
    col += this->fdPanelSize[f];

  f = panel;
  // shift out frames
  bitClear(this->fdCtrl, FD_COLUMN_SIG);
  bitClear(this->fdCtrl, FD_STROBE_SIG);
  bitSet  (this->fdCtrl, FD_FRAME_SIG);
  ShiftOut();
  delayMicroseconds(100);

  bitSet  (this->fdCtrl, fdPanelSelect[f]);
  bitClear(this->fdCtrl, FD_STROBE_SIG);
  bitClear(this->fdCtrl, FD_FRAME_SIG);
  ShiftOut();
  delayMicroseconds(190);

  for (int c = 0; c < this->fdPanelSize[f]; c++, col++)
  {
    //bitSet  (this->fdCtrl, FD_COLUMN_SIG);
    bitSet  (this->fdCtrl, FD_STROBE_SIG);
    ShiftOut();
    delayMicroseconds(15);

    this->fdCtrl &= ~FD_PANEL_MASK;
    bitClear(this->fdCtrl, FD_STROBE_SIG);
    bitClear(this->fdCtrl, FD_COLUMN_SIG);
    ShiftOut();
    delayMicroseconds(2100);

    //bitSet  (this->fdCtrl, FD_COLUMN_SIG);
    if ((this->fdMtx[0][col] != 0) || (this->fdMtx[1][col] != 0))
      bitSet  (this->fdCtrl, FD_COLUMN_SIG);
    else
      bitClear(this->fdCtrl, FD_COLUMN_SIG);
    ShiftOut();
    delayMicroseconds(200);

    bitSet  (this->fdCtrl, FD_STROBE_SIG);
    ShiftOut();
    delayMicroseconds(15);

    bitClear(this->fdCtrl, FD_STROBE_SIG);
    // for simpler hardware wiring flipdot pixel row are in reverse order (must be flipped bit wise)
    this->fdRow1 = bitReverse(fdMtx[0][col]);
    this->fdRow2 = bitReverse(fdMtx[1][col]);
    ShiftOut();
    delayMicroseconds(2000);

    this->fdRow1 = 0;
    this->fdRow2 = 0;
    ShiftOut();
    delayMicroseconds(200);

    bitClear(this->fdCtrl, FD_COLUMN_SIG);
    ShiftOut();
    delayMicroseconds(100);
  }
  delayMicroseconds(100);
  bitSet  (this->fdCtrl, FD_FRAME_SIG);
  ShiftOut();
}

void FlipDot::update(void)
{
  // initial state
  this->fdRow1 = 0x00;
  this->fdRow2 = 0x00;
  this->fdCtrl &= ~FD_PANEL_MASK;
  // shift out frames
  for (int f = 0; f < FD_PANELS; f++)
  {
    updatePanel(f);
  }
}

