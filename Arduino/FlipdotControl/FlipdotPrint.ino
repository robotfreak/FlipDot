/////////////////////////////////////////////////////////////////
//  Testenvironment and base for displaying Fonts on the
//  Flipdot display.
//  CC-BY SA NC 2016 c-hack.de    ralf@surasto.de
/////////////////////////////////////////////////////////////////

#include "font6x8v.h"
#include "font8x8.h"
#include "font8x12.h"
#include "font9x16.h"
#include "Flipdot.h"

//================== Constants ===============================
#define X_SIZE 50    // 115 column
#define Y_SIZE 2      // 16 rows (represented by 2 bytes)
#define Y_PIXELS 16   // True Y-Size if the display
#define OFF 0
#define ON 1

//================ global Variables ==========================
// frameBuffer stores the content of the Flipdotmatrix
// Each pixel is one bit (I.e only an eigth of storage is required
unsigned char frameBuffer[X_SIZE][Y_SIZE];


//#################### Public Functions ####################################

//====================================================
// Clears the entire framebuffer and flips every pixel
// of the display
// color = BLACK   all pixels set to black
// color = YELLOW  all pixels set to yellow
//====================================================
void clearFrameBuffer(int color) {
  int i, j;

  Serial.println("C");
  for (i = 0; i < X_SIZE; i++) {
    for (j = 0; j < Y_PIXELS; j++) {
      setFrameBuffer(i, j, color);
    }
  }
}

void updatePanel(void)
{
  printFrameBuffer();
  flipdot.update();
}
//====================================================
// Sets a Pixel at row Y column X
// color = BLACK   all pixels set to black
// color = YELLOW  all pixels set to yellow
//====================================================
void setPixel(int x, int y, int color) {

  setFrameBuffer(x, y, color);
}

int hex2int(char *hex) {
  int val = 0;
  while (*hex) {
    // get current character then increment
    uint8_t byte = *hex++;
    // transform hex character to the 4bit equivalent number, using the ascii table indexes
    if (byte >= '0' && byte <= '9') byte = byte - '0';
    else if (byte >= 'a' && byte <= 'f') byte = byte - 'a' + 10;
    else if (byte >= 'A' && byte <= 'F') byte = byte - 'A' + 10;
    // shift 4 to make space for new digit, and add the 4 bits of the new digit
    val = (val << 4) | (byte & 0xF);
  }
  return val;
}

//============================================
// printBitmap(int xOffs, int yOffs, int color, int xSize, int ySize, char *s)
// xOffs = position of the left side of the bitmap
// yOffs = position of the top of the bitmap
// color = ON means yellow, OFF means black
// xSize = horizontal size of the bitmap
// ySize = vertical sizw of the bitmap
// s = string
//============================================
int printBitmap(int xOffs, int yOffs, int color, int xSize, int ySize, String s) {
  int i, x, y, xs, ys, xt, yt, xo, yo, w;
  char stmp[3];

  i = 0;

  if (xSize > 0 && xSize <= X_SIZE && ySize > 0 && ySize <= Y_PIXELS )
  {
    while ((s[i] != '\0') && (i < 200))
    {
      ys = ySize;
      yo = 0;
      while (ys > 0)
      {
        if (ys < 8) yt = ys; else yt = 8;
        for (y = 0; y < yt; y++)
        {
          xs = xSize;
          xo = 0;
          while (xs > 0)
          {
            stmp[0] = s.charAt(i);
            stmp[1] = s.charAt(i+1);
            stmp[2] = 0;
            w = hex2int(stmp);
            //printf("w=%0x x=%d y=%d\n", w, xo, y+yo);
            i += 2;
            if (xs < 8) xt = xs; else xt = 8;
            for (x = 0; x < xt; x++)
            {
              if (w & 1) {
                setFrameBuffer(xt - 1 - x + xo + xOffs, y + yo + yOffs, color);
              }
              else {
                setFrameBuffer(xt - 1 - x + xo + xOffs, y + yo + yOffs, !color);
              }
              w = w >> 1;
            }
            xs -= xt;
            xo += xt;
          }
        }
        ys -= yt;
        yo += yt;
      }
    }
  }
  else printf("error size, x %d, y %d", xSize, ySize);
  return (x);
}


//====================================================
// Draws a horizotal line at row Y
// color = BLACK   all pixels set to black
// color = YELLOW  all pixels set to yellow
//===================================================
void hLine(int y, int color) {
  int i;

  for (i = 0; i < X_SIZE; i++) {
    setFrameBuffer(i, y, color);
  }
}

//====================================================
// Draws a vertical line at column X
// color = BLACK   all pixels set to black
// color = YELLOW  all pixels set to yellow
//====================================================
void vLine(int x, int color) {
  int i;

  for (i = 0; i < Y_PIXELS; i++) {
    setFrameBuffer(x, i, color);
  }
}


//============================================
// printString(int xOffs, int yOffs, int color, int size char s)
// xOffs = position of the left side of the string
// yOffs = position of the top of the string
// color = ON means yellow, OFF means black
// size = SMALL | MEDIUM | LARGE
// s = string
//============================================
int printString(int xOffs, int yOffs, int color, int size, String s) {
  int i, x, y;
  char c;

  i = 0;
  x = xOffs;
  y = yOffs;

  if (size == SMALL) x = xOffs - 2; // Somehow I need to shift the text to the left

  while ((i < s.length()) && (i < 100)) {
    switch (size) {
      case SMALL: x = printChar6x8v(x, y, color, s.charAt(i)); break;
      case MEDIUM: x = printChar8x8(x, y, color, s.charAt(i)); break;
      case LARGE: x = printChar8x12(x, y, color, s.charAt(i)); break;
      case XLARGE: x = printChar9x16v(x, y, color, s.charAt(i)); break;
      default: x = printChar6x8v(x, y, color, s[i]);
    }
    //Serial.print(s.charAt(i));
    i++;
  }
  return (x);
}


//###################### Internal Functions ##########################################


//===========================================
// setFrameBuffer(int x, int y, int value)
// Set one Pixel at x,y-Position
// value can be ON or OFF
//===========================================
void setFrameBuffer(int x, int y, int value) {
  unsigned char w, wNot;
  int yByteNo, yBitNo;

  w = 1;
  if ((y < 8 * Y_SIZE) && (x < X_SIZE) && (x >= 0) && (y >= 0)) {
    yByteNo = y / 8;  // integer division to select the byte
    yBitNo = y % 8;   // modulo division (residual) to select the bit in that byte
    w = w << yBitNo;
    if (value == ON) {
      frameBuffer[x][yByteNo] = frameBuffer[x][yByteNo] | w; // Logical OR adds one bit to the existing byte
    } else {
      wNot = 0xFF - w;
      frameBuffer[x][yByteNo] = frameBuffer[x][yByteNo] & wNot; // Logical AND set one bit to zero in the existing byte
    }
    flipdot.setPixel(x, y, value);
  }
}

//===========================================
// int getFrameBuffer(int x, int y)
// Gets color of one Pixel at x,y-Position
// returns value can be ON or OFF
//===========================================
int getFrameBuffer(int x, int y) {
  unsigned char w, wNot;
  int yByteNo, yBitNo;
  int value = 0;

  w = 1;
  if ((y < 8 * Y_SIZE) && (x < X_SIZE) && (x >= 0) && (y >= 0)) {
    yByteNo = y / 8;  // integer division to select the byte
    yBitNo = y % 8;   // modulo division (residual) to select the bit in that byte
    w = w << yBitNo;
    if (frameBuffer[x][yByteNo] & w) value = 1; else value = 0;
  }
  return value;
}


//============================================
// printChar6x8(int xOffs, int yOffs, char c)
// printChar8x8(int xOffs, int yOffs, char c)
// printChar8x12(int xOffs, int yOffs, char c)
//
// xOffs = position of the left side of the character
// yOffs = position of the top of the character
// color = ON means yellow, OFF means black
// c = ASCII Character
// returns new x position
//============================================
#if 0
int printChar6x8(int xOffs, int yOffs, int color, unsigned char c) {
  unsigned char x, y, w;

  for (y = 0; y < 8; y++) {
    w = pgm_read_byte(&(font6x8[c][y]));  // Important: pgm_read_byte reads from the array in the flash memory
    for (x = 0; x < 8; x++) {
      if (w & 1) {
        setFrameBuffer(x+xOffs,y+yOffs,color);
      }
      w = w >> 1;
    }
  }
  return (xOffs + 6);
}
#endif
int printChar6x8v(int xOffs, int yOffs, int color, unsigned char c) {
  unsigned int x, y, w, ctmp;
  ctmp = c - 32;
  for (x = 0; x < 8; x++) {
    w = pgm_read_byte(&(font6x8v[ctmp][x]));
    for (y = 0; y < 8; y++) {
      if (w & 1) {
        setFrameBuffer(x + xOffs, 7 - y + yOffs, color);
      }
      else {
        setFrameBuffer(x+xOffs,7-y+yOffs,!color);
      }
      w = w >> 1;
    }
  }
  return (xOffs + 6);
}


int printChar8x8(int xOffs, int yOffs, int color, unsigned char c) {
  unsigned int x, y, w;

  for (y = 0; y < 8; y++) {
    w = pgm_read_byte(&(font8x8[c][y])); // Important: pgm_read_byte reads from the array in the flash memory
    for (x = 0; x < 8; x++) {
      if (w & 1) {
        setFrameBuffer(x+xOffs,y+yOffs,color);
      }
      else {
        setFrameBuffer(x+xOffs,y+yOffs,!color);
      }
      w = w >> 1;
    }
  }
  return (xOffs + 8);
}


int printChar8x12(int xOffs, int yOffs, int color, unsigned char c) {
  unsigned int x, y, w;

  for (y = 0; y < 12; y++) {
    w = pgm_read_byte(&(font8x12[c][y])); // Important: pgm_read_byte reads from the array in the flash memory
    for (x = 0; x < 12; x++) {
      if (w & 1) {
        setFrameBuffer(x+xOffs,y+yOffs,color);
      }
      else {
        setFrameBuffer(x+xOffs,y+yOffs,!color);
      }
      w = w >> 1;
    }
  }
  return (xOffs + 8);
}

int printChar9x16v(int xOffs, int yOffs, int color, unsigned char c) {
  unsigned int x, y, wL, wH, ctmp;
  ctmp = c - 32;
  if (ctmp >= 64) ctmp -= 32;
  if (ctmp >= 32) ctmp += 32;
  else if (ctmp >= 64) ctmp += 64;
  for (x = 0; x < 9; x++) {
    wL = pgm_read_byte(&(font9x16v[ctmp * FONT_WIDTH + x]));
    wH = pgm_read_byte(&(font9x16v[ctmp * FONT_WIDTH + FONT_WIDTH * 32 + x]));
    for (y = 0; y < 8; y++) {
      if (wH & 1) {
        setFrameBuffer(x + xOffs, 7 - y + yOffs, color);
      }
      else {
        setFrameBuffer(x+xOffs,7-y+yOffs,!color);
      }
      wH = wH >> 1;

      if (wL & 1) {
        setFrameBuffer(x + xOffs, 15 - y + yOffs, color);
      }
      else {
        setFrameBuffer(x+xOffs,15-y+yOffs,!color);
      }
      wL = wL >> 1;
    }
  }
  return (xOffs + 11);
}

//============================================
// DEBUG ONLY
// printFrameBuffer is only used to see the
// content on the screen for debug
//============================================
void printFrameBuffer() {
  int x, y, bitNo, maxBits;
  unsigned char w;

  maxBits = 8;
  for (y = 0; y < Y_SIZE; y++) {
    w = 1;   // most right bit set
    if (y == Y_SIZE - 1) {
      maxBits = 8 - Y_SIZE * 8 % Y_PIXELS;
    }
    for (bitNo = 0; bitNo < maxBits; bitNo++) {
      for (x = 0; x < X_SIZE; x++) {
        if (frameBuffer[x][y] & w) Serial.print("#"); else Serial.print(".");
      }
      w = w << 1;
      Serial.println("");
    }
  }
}


