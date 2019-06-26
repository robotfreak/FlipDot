#include "Flipdot.h"
#include "U8glib.h"
#include <stdlib.h>
#include <stdio.h>

//#define LATCH 10
//#define OE 9

//FlipDot fd(LATCH, OE);
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

#define SCMD_SOT   0x0080
#define SCMD_CMD56 0x0085
#define SCMD_PAN1  0x0001
#define SCMD_PAN2  0x0002
#define SCMD_PAN3  0x0003
#define SCMD_PAN4  0x0004
#define SCMD_PAN5  0x0005
#define SCMD_EOT   0x008F

enum eSCmdState {
  SCS_INIT,     // 0
  SCS_GET_SOT,  // 1
  SCS_GET_CMD,  // 2
  SCS_GET_PAN,  // 3
  SCS_GET_DAT,  // 4
  SCS_END_DAT,  // 5
  SCS_GET_EOT,  // 6
  SCS_ERR       // 7
};

enum eSCmdState SCState = SCS_INIT;
int inBuf[128];
int inPt = 0;
int selPanel = 0;


void copyDat2Col(int ofs, int dat)
{
  if (ofs < 28)
  {
    for (int c = 0; c < 7; c++)
    {
      if (dat &  (1 << c))
        flipdot.set(ofs, c, 1);
      else
        flipdot.set(ofs, c, 0);

    }
  }
  else if (ofs >= 28 && ofs < 56)
  {
    for (int c = 0; c < 7; c++)
    {
      if (dat &  (1 << c))
        flipdot.set(ofs-28, c+7, 1);
      else
        flipdot.set(ofs-28, c+7, 0);
    }
  }
  else if (ofs >= 56 && ofs < 84)
  {
    for (int c = 0; c < 2; c++)
    {
      if (dat &  (1 << c))
        flipdot.set(ofs-56, c+14, 1);
      else
        flipdot.set(ofs-56, c+14, 0);
    }
  }
}

int parseSerial(int dat)
{
  int ret = 0;
  int err = 0;
  int ofs;

  switch (dat)
  {
    case SCMD_SOT:
      SCState = SCS_INIT;
    break;
    case SCMD_EOT:
      SCState = SCS_END_DAT;
    break;
    
    
  }
  switch (SCState)
  {
    case SCS_INIT:
      if (dat == SCMD_SOT)
      {
        SCState = SCS_GET_SOT;
        inPt = 0;
      }
      else
      {
//        err = -1;
        SCState = SCS_INIT;
      }
      break;
    case SCS_GET_SOT:
      //Serial.println("Got SOT");
      if (dat == SCMD_CMD56) 
        SCState = SCS_GET_CMD;
      else
      {
        err = -2;
        SCState = SCS_INIT;
      }
      break;
    case SCS_GET_CMD:
      //Serial.println("Got CMD");
      if (dat >= SCMD_PAN1 && dat <= SCMD_PAN5)
      {
        SCState = SCS_GET_DAT;
        selPanel = dat;
      }
      else
      {
        err = -3;
        SCState = SCS_INIT;
      }
      break;
    case SCS_GET_DAT:
      //Serial.println("Got DAT");
      if (inPt < 28)
      {
        if (selPanel == 3) 
          ofs = 56; 
        if (selPanel == 2) 
          ofs = 28; 
        else ofs = 0;
        copyDat2Col(inPt+ofs, dat);
        inBuf[inPt++] = dat;
      }
      else SCState = SCS_END_DAT;
      break;
    case SCS_END_DAT:
      if (dat == SCMD_EOT)
      {
        SCState = SCS_INIT;
        ret = selPanel;
      }
      else
      {
        err = -4;
        SCState = SCS_INIT;
      }
      break;
    default:
      SCState = SCS_INIT;
      break;
  }
  if (err)
    ret = err;
  return ret;
}

void setup() {
  int x,y;
  Serial.begin(9600);
  Serial.println ("FlipDot U8G Serial Test v1.0");
  while (!Serial);

  flipdot.begin();
//  u8g_Init(&u8g, &u8g_dev_flipdisc_2x7);
//  u8g_SetFlipDiscCallback(&u8g, writeFlipDiscMatrix);
//  u8g_FirstPage(&u8g);
  for(x=0;x<28;x++)
    for(y=0;y<16;y++)
      flipdot.set(x, y, 1);
  flipdot.update();
  delay(3000);
      
}

int inByte = 0;
  int ret;

void loop() {
}


void serialEvent() {
  
  while (Serial.available() > 0)
  {
    inByte = Serial.read();
    ret = parseSerial(inByte);
    Serial.print(inByte, HEX);
    Serial.print(", ");
    if (ret > 0)
    {
      Serial.println("OK");
      flipdot.update();
//      flipdot.updatePanel(0);
//      flipdot.updatePanel(1);
    }
    else if (ret < 0)
    {
      Serial.print("Err: ");
      Serial.print(ret);
      Serial.print(", ");
      Serial.println(inByte, HEX);
    }

  }
}
