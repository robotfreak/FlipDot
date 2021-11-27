#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <SPI.h>

#include "Flipdot.h"
#if 0
FlipDot::FlipDot()
{
  this->sizeX = FD_COLUMS;
  this->sizeY = FD_ROWS;
  //  this->invert = invert;
}
#endif
FlipDot::FlipDot(int16_t _sizeX, int16_t _sizeY) :
        Adafruit_GFX(_sizeX, _sizeY)
{
  this->sizeX = _sizeX;
  this->sizeY = _sizeY;
  this->panels = FD_PANELS;
  this->onTime = 2000;

//  this->invert = invert; 
  displayBuffer = (boolean*) calloc(FD_COLUMS*2, sizeof(boolean));
}

void FlipDot::begin(int8_t latch, int8_t oe, int8_t col, int8_t comm)
{
  this->latchPin = latch;
  this->oePin = oe;
  this->colPin = col;
  this->comPin = comm;

  memset(fdMtx, 0x00, sizeof(fdMtx));

  SPI.begin ();
  pinMode(this->latchPin, OUTPUT);
  pinMode(this->oePin, OUTPUT);
  pinMode(this->colPin, OUTPUT);
  pinMode(this->comPin, OUTPUT);
  delayMicroseconds(100);
  bitSet  (this->fdCtrl, FD_FRAME_SIG);
  ShiftOut();
  digitalWrite (this->oePin, LOW);
  digitalWrite (this->colPin, HIGH);
  digitalWrite(this->comPin, HIGH);

}

void FlipDot::begin(void)
{
  begin(LATCH,OE,COL,COMM);
}


void FlipDot::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (x >= this->sizeX || y >= this->sizeY) return;
  displayBuffer[y*this->sizeY+x] = (color);
  set(x,y,color);
} 

void FlipDot::setPanels(int16_t cnt) {
  if (cnt > 0 && cnt <= FD_PANELS)
    this->panels = cnt;
}

void FlipDot::setOnTime(int time)
{
  this->onTime = time;
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

void FlipDot::set(int16_t x, int16_t y, uint16_t color)
{
  //  int y2dot[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
  if (y < FD_ROWS && x < FD_COLUMS && y >= 0 && x >= 0)
  {
    int idx, dot;
    if (y > 7)
    {
      idx = 1;
      dot = (1 << (y - 8));
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
}

void FlipDot::setColumn(uint8_t x, uint16_t y, uint8_t dat)
{
  if (x < FD_COLUMS && y < 2)
  {
    this->fdMtx[y][x] = dat;
  }
}

void FlipDot::ShiftOut(void)
{
  SPI.beginTransaction(SPISettings(400000, MSBFIRST, SPI_MODE0));
  digitalWrite (this->latchPin, LOW);
  SPI.transfer (~(this->fdCtrl));
  SPI.transfer (this->fdRow2);
  SPI.transfer (this->fdRow1);
  digitalWrite (this->latchPin, HIGH);
  SPI.endTransaction();
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
  digitalWrite (this->comPin, HIGH);
//  bitClear(this->fdCtrl, FD_COLUMN_SIG);
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
    digitalWrite (this->comPin, HIGH);
 //   bitClear(this->fdCtrl, FD_COLUMN_SIG);
    ShiftOut();
    delayMicroseconds(this->onTime);

    //bitSet  (this->fdCtrl, FD_COLUMN_SIG);
    if ((this->fdMtx[0][col] != 0) || (this->fdMtx[1][col] != 0))
    {
     digitalWrite (this->comPin, LOW);
//     bitSet  (this->fdCtrl, FD_COLUMN_SIG);
    }
    else
    {
      digitalWrite (this->comPin, HIGH);
//      bitClear(this->fdCtrl, FD_COLUMN_SIG)
    }
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
    delayMicroseconds(this->onTime);

    this->fdRow1 = 0;
    this->fdRow2 = 0;
    ShiftOut();
    delayMicroseconds(200);

    digitalWrite (this->comPin, LOW);
//    bitClear(this->fdCtrl, FD_COLUMN_SIG);
    ShiftOut();
    delayMicroseconds(100);
  }
  delayMicroseconds(100);
  bitSet  (this->fdCtrl, FD_FRAME_SIG);
  ShiftOut();
}

void FlipDot::update(void)
{
  //digitalWrite (this->colPin, HIGH);
  //delay(50);
  // initial state
  this->fdRow1 = 0x00;
  this->fdRow2 = 0x00;
  this->fdCtrl &= ~FD_PANEL_MASK;
  // shift out frames
  for (int f = 0; f < this->panels; f++)
  {
    updatePanel(f);
  }
  //delay(50);
  //digitalWrite (this->colPin, LOW);
}

/*
void FlipDot::setLedColor(uint16_t red, uint16_t green, uint16_t blue)
{
  analogWrite(this->redPin, red);
  analogWrite(this->greenPin, green);
  analogWrite(this->bluePin, blue);
}
*/

