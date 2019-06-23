#include <Arduino.h>
#include "Flipdot.h"
#include "FlipdotUtils.h"

#define NUMROWS 16
#define NUMCOLS 50


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
  //printf("\nkey: %x\n", key);
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
  panelData[2 * x + y] = value;
}

uint16_t getPixel(int x, int y) {
  return panelData[x] & (1 << y);
}

void copyPixelData() {
  uint16_t xV, yV;
  uint16_t value;

  for (xV = 0; xV < PANELDATA_SIZE; xV++)
  {
    for (yV = 0; yV < Y_PIXELS; yV++)
    {
      value = (getPixel(xV, yV) != 0) ? ON : OFF;
      fdu.setFrameBuffer(xV, yV, value);
    }
  }
  fdu.printFrameBuffer();
}



uint8_t xg = PANELDATA_SIZE + 2;
int8_t yg = 6;

void drawTile(uint8_t x, uint8_t y, uint16_t* tile, uint8_t value) {
  if (value == 1) {
    if (x < PANELDATA_SIZE + 3) panelData[x - 3] |= (*tile >> 12) << y; //<< y >> -y;
    if (x < PANELDATA_SIZE + 2) panelData[x - 2] |= ((*tile & 0x0f00) >> 8) << y; // << y >> -y;
    if (x < PANELDATA_SIZE + 1) panelData[x - 1] |= ((*tile & 0x00f0) >> 4) << y; // << y >> -y;
    if (x < PANELDATA_SIZE + 0) panelData[x] |= (*tile & 0x000f)  << y; // << y >> -y;
  } else {
    if (x < PANELDATA_SIZE + 3) panelData[x - 3] &= ~((*tile >> 12) << y); // << y >> -y);
    if (x < PANELDATA_SIZE + 2) panelData[x - 2] &= ~(((*tile & 0x0f00) >> 8) << y); // << y >> -y);
    if (x < PANELDATA_SIZE + 1) panelData[x - 1] &= ~(((*tile & 0x00f0) >> 4) << y); // << y >> -y);
    if (x < PANELDATA_SIZE + 0) panelData[x] &= ~((*tile & 0x000f) << y); // << y >> -y);
  }
}

int collideTile(uint8_t x, uint8_t y, uint16_t* tile) {
  uint16_t coll = *tile >> 12;
  uint16_t mask = coll;
  if ((coll && x == 3) || (x < PANELDATA_SIZE + 4 && (panelData[x - 4] << -y /*>> y*/) & 0x0f & coll)) return 1;
  coll = ~mask & ((*tile & 0x0f00) >> 8);
  mask |= coll;
  if ((coll && x == 2) || (x < PANELDATA_SIZE + 3 && (panelData[x - 3] << -y /*>> y*/) & 0x0f & coll)) return 2;
  coll = ~mask & ((*tile & 0x00f0) >> 4);
  mask |= coll;
  if ((coll && x == 1) || (x < PANELDATA_SIZE + 2 && (panelData[x - 2] << -y /* >> y*/) & 0x0f & coll)) return 4;
  coll = ~mask & (*tile & 0x000f);
  mask |= coll;
  if ((coll && x == 0) || (x < PANELDATA_SIZE + 1 && (panelData[x - 1] << -y /*>> y*/) & 0x0f & coll)) return 8;
  return 0;
}

int block = 0;
int speed = 32;
int n = 0, logt = 1;

void Tetris() {
  fdu.clearFrameBuffer(OFF);
  fdu.printFrameBuffer();

  while (true) {
    //if(gameTick > 64) {
    //randCnt += buttonValue * 2 + 1;

    if (!collideTile(xg, yg, &blocks[block])) {
      drawTile(xg, yg, &blocks[block], 0);
      xg--;
    } else {
      // TODO: erase full lines
      //panelData[(n++)] |= collideTile(x, y, &blocks[block]) | (block << 10) | ((x>180) << 15);
      xg = PANELDATA_SIZE + 2;
      yg = 6;
      randCnt++;
      block = randCnt % 28;
      if (block > 28) block = 28;
    }
    buttonValue = getKey();
    if (buttonValue & (1 << LEFT_PIN)) {
      // TODO: collideY
      printf("y: %d\n", yg);
      yg++;
    }
    if (buttonValue & (1 << RIGHT_PIN)) {
      // TODO: collideY
      printf("y: %d\n", yg);
      yg--;
    }
    if (buttonValue & (1 << UP_PIN)) {
      // TODO: collideX & Y
      block = (block / 4) * 4 + (block + 1) % 4;
    }
    if (buttonValue & (1 << DOWN_PIN)) {
      while (!collideTile(xg, yg, &blocks[block])) {
        xg--;
      }
    }
    buttonValue &= ~((1 << LEFT_PIN) | (1 << RIGHT_PIN) | (1 << UP_PIN) | (1 << DOWN_PIN));
    //randCnt /= 3;
    drawTile(xg, yg, &blocks[block], 1);
    copyPixelData();
    //gameTick-=64; // 16=normal
    //}
    gameTick++;
    fdu.updatePanel();
    printf("%d\n", gameTick);
    delay(50);
  }
}
