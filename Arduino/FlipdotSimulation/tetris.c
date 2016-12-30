/////////////////////////////////////////////////////////////////
//  Conway's Game of Life on the Flipdot display.
//  Game of Life based on:
//  http://brownsofa.org/blog/2010/12/30/arduino-8x8-game-of-life/
//  Flipdot Simulator based on:
//  CC-BY SA NC 2016 c-hack.de    ralf@surasto.de
//  Porting to Annax Flipdot:
//  by RobotFreak  webmaster@robotfreak.de
/////////////////////////////////////////////////////////////////
#ifdef _WIN32
#include <windows.h> 
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#endif

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "font6x8v.h"

//================== Constants ===============================
#define X_SIZE 50   // 128 column
#define Y_SIZE 2    // 28 rows (represented by 4 bytes)
#define Y_PIXELS 16 // True Y-Size if the display
#define OFF 0
#define ON 1
#define XSMALL 0
#define SMALL 1
#define MEDIUM 2
#define LARGE 3
#define XLARGE 4

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

//================ global Variables ==========================
// frameBuffer stores the content of the Flipdotmatrix
// Each pixel is one bit (I.e only an eigth of storage is required
uint8_t frameBuffer[X_SIZE][Y_SIZE];

//===========================================
// clearFrameBuffer(int color)
// Sets all bits to
//     Yellow if color = ON
//     Black if color  = OFF
//===========================================
void clearFrameBuffer(int color)
{
  if (color == ON)
    memset(frameBuffer, -1, sizeof(frameBuffer));
  else
    memset(frameBuffer, 0, sizeof(frameBuffer));
}

//===========================================
// setFrameBuffer(int x, int y, int value)
// Set one Pixel at x,y-Position
// value can be ON or OFF
//===========================================
void setFrameBuffer(int x, int y, int value)
{
  unsigned char w, wNot;
  int yByteNo, yBitNo;

  w = 1;
  if ((y < 8 * Y_SIZE) && (x < X_SIZE) && (x >= 0) && (y >= 0))
  {
    yByteNo = y / 8; // integer division to select the byte
    yBitNo = y % 8;  // module division (residual) to select the bit in that byte
    w = w << yBitNo;
    if (value == ON)
    {
      frameBuffer[x][yByteNo] = frameBuffer[x][yByteNo] | w; // Logical OR adds one bit to the existing byte
    }
    else
    {
      wNot = 0xFF - w;
      frameBuffer[x][yByteNo] = frameBuffer[x][yByteNo] & wNot; // Logical AND set one bit to zero in the existing byte
    }
  }
}

//===========================================
// getFrameBuffer(int x, int y)
// Get one Pixel at x,y-Position
// returns value can be ON or OFF
//===========================================
int getFrameBuffer(int x, int y)
{
  unsigned char w, wNot;
  int yByteNo, yBitNo;
  int value = 0;

  w = 1;
  if ((y < 8 * Y_SIZE) && (x < X_SIZE) && (x >= 0) && (y >= 0))
  {
    yByteNo = y / 8; // integer division to select the byte
    yBitNo = y % 8;  // module division (residual) to select the bit in that byte
    w = w << yBitNo;
    if (frameBuffer[x][yByteNo] & w) value = 1; else value = 0;
  }
  return value;
}


//============================================
// printCharXxY(int xOffs, int yOffs, char c)
// xOffs = position of the left side of the character
// yOffs = position of the top of the character
// color = ON means yellow, OFF means black
// c = ASCII Character
// returns new x position
//============================================

int printChar6x8v(int xOffs, int yOffs, int color, unsigned char c)
{
  unsigned char x, y, w, ctmp;
  ctmp = c - 32;
  for (x = 0; x < 6; x++)
  {
    w = font6x8v[ctmp][x];
    for (y = 0; y < 8; y++)
    {
      if (w & 1)
        setFrameBuffer(x + xOffs, 7 - y + yOffs, color);
      w = w >> 1;
    }
  }
  return (xOffs + 6);
}

//============================================
// printString(int xOffs, int yOffs, char s)
// xOffs = position of the left side of the string
// yOffs = position of the top of the string
// color = ON means yellow, OFF means black
// s = string
//============================================
int printString(int xOffs, int yOffs, int color, int size, const char *s)
{
  int i, x, y;

  i = 0;
  x = xOffs;
  y = yOffs;
  while ((s[i] != '\0') && (i < 200))
  {
    x = printChar6x8v(x, y, color, s[i]);
    i++;
  }
  return (x);
}

int hex2int(char *hex)
{
    int val = 0;
    while (*hex)
    {
        // get current character then increment
        uint8_t byte = *hex++;
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9')
            byte = byte - '0';
        else if (byte >= 'a' && byte <= 'f')
            byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <= 'F')
            byte = byte - 'A' + 10;
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
int printBitmap(int xOffs, int yOffs, int color, int xSize, int ySize, const char *s)
{
    int i, x, y, xs, ys, xt, yt, xo, yo, w;
    char stmp[3];

    i = 0;

    if (xSize > 0 && xSize <= X_SIZE && ySize > 0 && ySize <= Y_PIXELS)
    {
        while ((s[i] != '\0') && (i < 200))
        {
            ys = ySize;
            yo = 0;
            while (ys > 0)
            {
                if (ys < 8)
                    yt = ys;
                else
                    yt = 8;
                for (y = 0; y < yt; y++)
                {
                    xs = xSize;
                    xo = 0;
                    while (xs > 0)
                    {
                        stmp[0] = s[i];
                        stmp[1] = s[i + 1];
                        stmp[2] = 0;
                        w = hex2int(stmp);
                        i += 2;
                        if (xs < 8)
                            xt = xs;
                        else
                            xt = 8;
                        for (x = 0; x < xt; x++)
                        {
                            if (w & 1)
                            {
                                //printf("x=%d y=%d\n",xt - 1 - x + xo + xOffs,  y + yo + yOffs);
                                setFrameBuffer(xt - 1 - x + xo + xOffs, y + yo + yOffs, color);
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
    else
        printf("error size, x %d, y %d", xSize, ySize);
    return (x);
}

//===========================================
// setCursorPos(int xPos, int yPos)
// Set the cursor at x,y-Position
//===========================================
void setCursorPos(int xPos, int yPos)
{
#ifdef _WIN32
  COORD coord;
  coord.X = xPos;
  coord.Y = yPos;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
  printf("\033[%d;%dH", yPos + 1, xPos + 1);
#endif
}

//===========================================
// clearScreen()
// Clear the screen
//===========================================
void clearScreen()
{
#ifdef _WIN32
  system("cls");
#else
  printf("\033[2J");
#endif
}

//============================================
// DEBUG ONLY
// printFrameBuffer is only used to see the
// content on the screen for debug
//============================================
void printFrameBuffer()
{
  int x, y, bitNo, maxBits;
  unsigned char w;

  maxBits = 8;
  setCursorPos(0, 0);

  for (y = 0; y < Y_SIZE; y++)
  {
    w = 1; // most right bit set
    if (y == Y_SIZE - 1)
    {
      maxBits = 8 - Y_SIZE * 8 % Y_PIXELS;
    }
    for (bitNo = 0; bitNo < maxBits; bitNo++)
    {
      for (x = 0; x < X_SIZE; x++)
      {
        if (frameBuffer[x][y] & w)
          printf("#");
        else
          printf(".");
      }
      w = w << 1;

      printf("\n");
    }
  }
}


// *******************************************************
// *    Tetris functions                                 *
// *******************************************************
#define PANELDATA_SIZE 50

#define LEFT_PIN 0
#define RIGHT_PIN 1
#define UP_PIN 2
#define DOWN_PIN 3

uint16_t blocks[] = {
//  ### 
//   #  
  0x0E40, 0x4C40, 0x4E00, 0x4640,
// ####
//     
  0x0F00, 0x2222, 0x00F0, 0x4444,
// ##  
//  ## 
  0x0C60, 0x4C80, 0xC600, 0x2640,
// ### 
// #   
  0x4460, 0x0E80, 0xC440, 0x2E00,
// ### 
//   # 
  0x44C0, 0x8E00, 0x6440, 0x0E20,
//  ## 
// ##  
  0x06C0, 0x8C40, 0x6C00, 0x4620,
// ##  
// ##  
  0x0CC0, 0x0CC0, 0x0CC0, 0x0CC0
};

uint16_t randCnt;
uint16_t gameTick = 0;

uint8_t buttonValue = 0;
uint8_t buttonStatus = 0;

uint16_t panelData[PANELDATA_SIZE];

uint8_t getKey(void)
{
uint8_t key = 0;

#ifdef _WIN32
if(GetAsyncKeyState(VK_LEFT)&1 == 1)
{
  key |= (1<< LEFT_PIN);  
}
else if(GetAsyncKeyState(VK_RIGHT)&1 == 1)
{
  key |= (1<< RIGHT_PIN);  
}
else if(GetAsyncKeyState(VK_UP)&1 == 1)
{
  key |= (1<< UP_PIN);  
}
else if(GetAsyncKeyState(VK_DOWN)&1 == 1)
{
  key |= (1<< DOWN_PIN);  
}
#else
    int yalv;

    FILE *kbd = fopen("/dev/input/by-path/platform-i8042-serio-0-event-kbd", "r");

    char key_map[KEY_MAX/8 + 1];    //  Create a byte array the size of the number of keys

    memset(key_map, 0, sizeof(key_map));    //  Initate the array to zero's
    ioctl(fileno(kbd), EVIOCGKEY(sizeof(key_map)), key_map);    //  Fill the keymap with the current keyboard state
    for (yalv = 0; yalv < KEY_MAX; yalv++) {
      int bindex = yalv / 8;
      int b = yalv % 8;

      int result = key_map[bindex] & (1<<b);
    if (result) {
        /* the bit is set in the key state */
        printf("  Key 0x%02x ", yalv);
        switch ( yalv)
        {
        case KEY_PAGEUP :
            key |= (1<< UP_PIN);
        break;
        case KEY_PAGEDOWN :
           key |= (1<< DOWN_PIN);
        break;
        case KEY_LEFT:
           key |= (1<< LEFT_PIN);
        break;
        case KEY_RIGHT:
           key |= (1<< RIGHT_PIN);
        break;
        default:
            //printf(" (Unknown key)\n");
        break;
        }
    }
}
#endif
printf("\nkey: %x\n", key);
return key;
}

void clear() {
  uint16_t i;
  for (i = 0; i < PANELDATA_SIZE; i++) {
    panelData[i] = 0;
  }
}

void setPixel(uint16_t x, uint8_t y, uint8_t value) {
  if (value)
    panelData[x] |= 1 << y;
  else
    panelData[x] &= ~(1 << y);
}

void setRow(uint8_t x, uint8_t y, uint8_t value) {
  // TODO: Fixme
  panelData[2*x+y] = value;
}

uint16_t getPixel(int x, int y) {
  return panelData[x] & (1 << y);
}

void copyPixelData() {
  uint16_t xV, yV;
  uint16_t value; 

  for(xV=0; xV<PANELDATA_SIZE; xV++)
  {
    for(yV=0; yV<Y_PIXELS; yV++)
    {
      value = (getPixel(xV,yV) != 0) ? ON : OFF;
      setFrameBuffer(xV,yV,value);
    }
  }
  printFrameBuffer();
}

void setup() {
  clearScreen();
  clearFrameBuffer(OFF);
  printFrameBuffer();
}

uint8_t xg = PANELDATA_SIZE+2;
int8_t yg = 6;

void drawTile(uint8_t x, uint8_t y, uint16_t* tile, uint8_t value) {
  if(value == 1) {
    if(x < PANELDATA_SIZE+3) panelData[x-3] |= (*tile >> 12) << y; //<< y >> -y;
    if(x < PANELDATA_SIZE+2) panelData[x-2] |= ((*tile & 0x0f00) >> 8) << y; // << y >> -y;
    if(x < PANELDATA_SIZE+1) panelData[x-1] |= ((*tile & 0x00f0) >> 4) << y; // << y >> -y;
    if(x < PANELDATA_SIZE+0) panelData[x] |= (*tile & 0x000f)  << y; // << y >> -y;
  } else {
    if(x < PANELDATA_SIZE+3) panelData[x-3] &= ~((*tile >> 12) << y); // << y >> -y);
    if(x < PANELDATA_SIZE+2) panelData[x-2] &= ~(((*tile & 0x0f00) >> 8) << y); // << y >> -y);
    if(x < PANELDATA_SIZE+1) panelData[x-1] &= ~(((*tile & 0x00f0) >> 4) << y); // << y >> -y);
    if(x < PANELDATA_SIZE+0) panelData[x] &= ~((*tile & 0x000f) << y); // << y >> -y);
  }
}

int collideTile(uint8_t x, uint8_t y, uint16_t* tile) {
  uint16_t coll = *tile >> 12;
  uint16_t mask = coll;
  if((coll && x == 3) || (x < PANELDATA_SIZE+4 && (panelData[x-4] << -y /*>> y*/) & 0x0f & coll)) return 1;
  coll = ~mask & ((*tile & 0x0f00) >> 8);
  mask |= coll;
  if((coll && x == 2) || (x < PANELDATA_SIZE+3 && (panelData[x-3] << -y /*>> y*/) & 0x0f & coll)) return 2;
  coll = ~mask & ((*tile & 0x00f0) >> 4);
  mask |= coll;
  if((coll && x == 1) || (x < PANELDATA_SIZE+2 && (panelData[x-2] << -y /* >> y*/) & 0x0f & coll)) return 4;
  coll = ~mask & (*tile & 0x000f);
  mask |= coll;
  if((coll && x == 0) || (x < PANELDATA_SIZE+1 && (panelData[x-1] << -y /*>> y*/) & 0x0f & coll)) return 8;
  return 0;
}

int block = 0;
int speed = 32;
int n = 0, logt=1;

void loop() {
  //if(gameTick > 64) {
    //randCnt += buttonValue * 2 + 1;
    
    if(!collideTile(xg, yg, &blocks[block])) {
      drawTile(xg,yg,&blocks[block],0);
      xg--;
    } else {
      // TODO: erase full lines
      //panelData[(n++)] |= collideTile(x, y, &blocks[block]) | (block << 10) | ((x>180) << 15);
      xg = PANELDATA_SIZE+2;
      yg = 6;
      randCnt++;
      block = randCnt % 28;
      if(block > 28) block = 28;
    }
    buttonValue = getKey();
    if(buttonValue & (1<<LEFT_PIN)) {
      // TODO: collideY
      printf("y: %d\n", yg);
      yg++;
    }
    if(buttonValue & (1<<RIGHT_PIN)) {
      // TODO: collideY
      printf("y: %d\n", yg);
      yg--;
    }
    if(buttonValue & (1<<UP_PIN)) {
      // TODO: collideX & Y
      block = (block / 4) * 4 + (block + 1) % 4;
    }
    if(buttonValue & (1<<DOWN_PIN)) {
      while(!collideTile(xg, yg, &blocks[block])) {
        xg--;
      }
    }
    buttonValue &= ~((1<<LEFT_PIN) | (1<<RIGHT_PIN) | (1<<UP_PIN) | (1<<DOWN_PIN));
    //randCnt /= 3;
    drawTile(xg,yg,&blocks[block],1);
    copyPixelData();
    //gameTick-=64; // 16=normal
  //}
  gameTick++;
  printf("%d\n",gameTick);
  usleep(50000);
}


int main() {
  setup();
  //drawTile(40,6,&blocks[2],1);
  //copyPixelData();

  while(1) loop();
}
