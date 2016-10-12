#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <SPI.h>

#include "FlipDot.h"

FlipDot::FlipDot(int _latchPin, int _oePin)
{
  this->latchPin = _latchPin;
  this->oePin = _oePin;

  SPI.begin ();
  pinMode(this->latchPin, OUTPUT);
  pinMode(this->oePin, OUTPUT);
  digitalWrite(this->oePin, LOW);
}

void FlipDot::ShiftOut(void)
{
  digitalWrite (this->latchPin, LOW);
  SPI.transfer (~(this->fdCtrl));
  SPI.transfer (this->fdRow2);
  SPI.transfer (this->fdRow1);
  digitalWrite (this->latchPin, HIGH);
}


void FlipDot::UpdatePanel(int panel)
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
    this->fdRow1 = fdMtx[0][col];
    this->fdRow2 = fdMtx[1][col];
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

void FlipDot::Update(void)
{
  // initial state
  this->fdRow1 = 0x00;
  this->fdRow2 = 0x00;
  this->fdCtrl &= ~FD_PANEL_MASK;
  // shift out frames
  for (int f = 0; f < FD_PANELS; f++)
  {
    UpdatePanel(f);
  }
}

