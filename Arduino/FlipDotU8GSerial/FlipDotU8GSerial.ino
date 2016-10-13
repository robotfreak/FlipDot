#include "FlipDot.h"
#include "U8glib.h"
#include <stdlib.h>
#include <stdio.h>

#define LATCH 10
#define OE 9

FlipDot fd(LATCH, OE);
int fontWidth, fontHeight;

#define DEBUG

FlipDot flipdot(50, FD_ROWS);

void writeFlipDiscMatrixRow(uint8_t width, uint8_t *row, uint8_t ofs)
{
  uint8_t i, b;
  for ( b = 0; b < 7; b++ )
  {
    for ( i = 0; i < width; i++ )
    {
      if ( row[i] & (1 << b) )
      {
#ifdef DEBUG
        Serial.print("#");
#endif
        flipdot.drawPixel(i, b + (ofs * 7), FLIPDOT_YELLOW);
      }
      else
      {
#ifdef DEBUG
        Serial.print(".");
#endif
        flipdot.drawPixel(i, b + (ofs * 7), FLIPDOT_BLACK);
      }
    }
#ifdef DEBUG
    Serial.print("\n");
#endif
  }
}


/*
  Write data to the flip disc matrix.
  This procedure must be implemented by the user.
  Arguments:
    id: Id for the matrix. Currently always 0.
    page:   A page has a height of 14 pixel. For a matrix with HEIGHT == 14 this will be always 0
    width:  The width of the flip disc matrix. Always equal to WIDTH
    row1:   first data line (7 pixel per byte)
    row2:   first data line (7 pixel per byte)
*/
void writeFlipDiscMatrix(uint8_t id, uint8_t page, uint8_t width, uint8_t *row1, uint8_t *row2)
{
  writeFlipDiscMatrixRow(width, row1, 0);
  writeFlipDiscMatrixRow(width, row2, 1);
}

u8g_t u8g;

#define SCMD_SOT   0x80
#define SCMD_CMD56 0x85
#define SCMD_PAN1  0x01
#define SCMD_PAN2  0x02
#define SCMD_PAN3  0x03
#define SCMD_PAN4  0x04
#define SCMD_PAN5  0x05
#define SCMD_EOT   0x8F

enum eSCmdState {
  SCS_INIT,
  SCS_GET_SOT,
  SCS_GET_CMD,
  SCS_GET_PAN,
  SCS_GET_DAT,
  SCS_END_DAT,
  SCS_GET_EOT
};

enum eSCmdState SCState = SCS_INIT;
int inBuf[56];
int inPt = 0;
int selPanel = 0;


void copyDat2Col(int ofs, int dat)
{
  if (ofs < 25)
  {
    for (int c = 0; c < 8; c++)
    {
      if (dat &  (1 << c))
        u8g_DrawPixel(&u8g, ofs, c);
    }
  }
  else if (ofs >= 28 && ofs < 56)
  {
    for (int c = 0; c < 8; c++)
    {
      if (dat &  (1 << c))
        u8g_DrawPixel(&u8g, ofs-28, c+8);
    }
  }
}

int parseSerial(int dat)
{
  int ret = 0;
  switch (SCState)
  {
    case SCS_INIT:
      if (dat == SCMD_SOT) 
      {
        SCState = SCS_GET_SOT;
        inPt = 0;
      }
      else SCState = SCS_INIT;
    break;    
    case SCS_GET_SOT:
      //Serial.println("Got SOT");
      if (dat == SCMD_CMD56) SCState = SCS_GET_CMD;
      else SCState = SCS_INIT;
    break;    
    case SCS_GET_CMD:
      //Serial.println("Got CMD");
      if (dat >= SCMD_PAN1 && dat <= SCMD_PAN5) 
      {
        SCState = SCS_GET_PAN;
        selPanel = dat;
      }
      else SCState = SCS_INIT;
    break;    
    case SCS_GET_PAN:
    case SCS_GET_DAT:
      //Serial.println("Got DAT");
      if (inPt < 56) 
      {
        copyDat2Col(inPt, dat);
        inBuf[inPt++] = dat;
        SCState = SCS_GET_DAT;
      }
      else SCState = SCS_END_DAT;
    break;    
    case SCS_END_DAT:
      //Serial.println("Got EOT");
      if (dat == SCMD_EOT) 
      {
        SCState = SCS_INIT;
        ret = selPanel;
      }
      else SCState = SCS_INIT;
    break;    
  }
  return ret;
}

void setup() {

  Serial.begin(115200);
  Serial.println ("FlipDot U8G Serial Test v1.0");
  while (!Serial);

  flipdot.begin(LATCH, OE);
  u8g_Init(&u8g, &u8g_dev_flipdisc_2x7);
  u8g_SetFlipDiscCallback(&u8g, writeFlipDiscMatrix);
  u8g_FirstPage(&u8g);
}

void loop() {
  int inByte;
  int ret;
  while (Serial.available() > 0)
  {
    inByte = Serial.read();
    ret = parseSerial(inByte);
    if (ret > 0)
    {
      flipdot.updatePanel(ret-1);
    }
  }
}


