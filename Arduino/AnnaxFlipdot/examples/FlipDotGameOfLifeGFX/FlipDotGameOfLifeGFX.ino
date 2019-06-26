/*********************************************************************
  This sketch uses example for Adafruit Monochrome OLEDs based on SSD1306 drivers
  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98
  This example is for a 128x32 size display using I2C to communicate
  3 pins are required to interface (2 I2C and one reset)
  
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Limor Fried/Ladyada  for Adafruit Industries.
  BSD license, check license.txt for more information
  All text above, and the splash screen must be included in any redistribution

  Adopted for Annax Flipdot by robotfreak
*********************************************************************/

// Button Pin Setup
const int l_up_button = 3; //Left player up button pin
const int l_down_button = 6; //Left player down button pin
const int r_up_button = 7; //Right player up button pin
const int r_down_button = 8; //Right player down button pin


//Screen Size Parameters
#define NUMROWS 16
#define NUMCOLS 50

int x_pixels = NUMCOLS;
int y_pixels = NUMROWS;

uint8_t newGameBoard[NUMCOLS][NUMROWS/8];


#include <Adafruit_GFX.h>
#include "Flipdot.h"
#include "FlipdotUtils.h"

FlipDot flipdot(50, FD_ROWS);
FlipDotUtils fdu(flipdot);

struct gameInitializer {
  int xOffs;
  int yOffs;
  int xSize;
  int ySize; 
  const char *s;
};

struct gameInitializer gi[7] = {
  {24, 6, 4, 4, "03060200"},
  {24, 6, 8, 8, "2008670000000000"},
  {24, 4, 4, 8, "0705050005050700"},
  {24, 2, 8, 4, "02C04700"},
  {20, 8, 8, 6, "020b0a0820a0"},
  {20, 8, 5, 5, "1D10030D15"},
  {1, 8, 40, 1, "7FBE381FDF"}
};

void setup() {
  int gidx;
  pinMode(l_up_button, INPUT);
  pinMode(l_down_button, INPUT);
  pinMode(r_up_button, INPUT);
  pinMode(r_down_button, INPUT);
  Serial.begin(9600);

  flipdot.begin();
  fdu.setSerialDebug(false);
  flipdot.setPanels(2);
  flipdot.setOnTime(1200);

  //flipdot.display();
  flipdot.update();
  // Clear the buffer.
  flipdot.fillScreen(BLACK);;
  //flipdot.update();
  // Display Arduino Pong splashscreen
  flipdot.setTextSize(1);
  flipdot.setTextColor(YELLOW);
  flipdot.setCursor(flipdot.width() / 2 - 20, 0);
  flipdot.println("Game of");
  flipdot.setCursor(flipdot.width() / 2 - 20 / 2, 8);
  flipdot.println("Life");
  //flipdot.display();
  flipdot.update();
  delay(2000);

  fdu.clearFrameBuffer(OFF);
  fdu.updatePanel();
  delay(1000);
  gidx = random(7);

  initGameBoard(gi[gidx].xOffs, gi[gidx].yOffs, 
                gi[gidx].xSize, gi[gidx].ySize,
                gi[gidx].s);
  //initGameBoard(24, 6, 4, 4, "03060200");  // r-pentomino
  //initGameBoard(24, 6, 8, 8, "2008670000000000");   // acorn
  //initGameBoard(24, 4, 4, 8, "0705050005050700");   // 
  //initGameBoard(24, 2, 8, 4, "02C04700");   // die hard
  //initGameBoard(20, 8, 8, 6, "020b0a0820a0");   // infinity
  //initGameBoard(20, 8, 5, 5, "1D10030D15");   // infinity2
  //initGameBoard(1, 8, 40, 1, "7FBE381FDF");   // infinity3
  //initGameBoard();
  fdu.updatePanel();
  delay(100);

}

void loop() {
    //clearAll(OFF);
    showGameBoard();
    //printFrameBuffer();
    fdu.updatePanel();
//    Serial.println(iterations, DEC);

}

//===========================================
// setNewGameBoard(int x, int y, int value)
// Set one Pixel at x,y-Position
// value can be ON or OFF
//===========================================
void setNewGameBoard(int x, int y, int value)
{
  unsigned char w, wNot;
  int yByteNo, yBitNo;

  w = 1;
  if ((y < NUMROWS) && (x < NUMCOLS) && (x >= 0) && (y >= 0))
  {
    yByteNo = y / 8; // integer division to select the byte
    yBitNo = y % 8;  // module division (residual) to select the bit in that byte
    w = w << yBitNo;
    if (value == ON)
    {
      newGameBoard[x][yByteNo] = newGameBoard[x][yByteNo] | w; // Logical OR adds one bit to the existing byte
    }
    else
    {
      wNot = 0xFF - w;
      newGameBoard[x][yByteNo] = newGameBoard[x][yByteNo] & wNot; // Logical AND set one bit to zero in the existing byte
    }
  }
}

//===========================================
// getNewFrameBuffer(int x, int y)
// Get one Pixel at x,y-Position
// returns value can be ON or OFF
//===========================================
int getNewGameBoard(int x, int y)
{
  unsigned char w, wNot;
  int yByteNo, yBitNo;
  int value = 0;

  w = 1;
  if ((y < NUMROWS) && (x < NUMCOLS) && (x >= 0) && (y >= 0))
  {
    yByteNo = y / 8; // integer division to select the byte
    yBitNo = y % 8;  // module division (residual) to select the bit in that byte
    w = w << yBitNo;
    if (newGameBoard[x][yByteNo] & w) value = 1; else value = 0;
  }
  return value;
}

void perturbInitialGameBoard() {
  int row, col, val;
  int numChanges, i;

  numChanges = random(20, 100);
  printf("numChanges: %d\n", numChanges);
  for ( i = 0; i < numChanges; i++) {
    row = random(0, NUMROWS);
    col = random(0, NUMCOLS);
    val = fdu.getFrameBuffer(col, row);    
    if (val)
      fdu.setFrameBuffer(col,row, 0);
    else
      fdu.setFrameBuffer(col,row, 1);
  }
}

//===========================================
// initGameBoard()
// Initialises the game board 
// set the start position bitmap
//===========================================
int initGameBoard(int xOffs, int yOffs, int xSize, int ySize, const char *s)
//void initGameBoard()
{
  fdu.clearFrameBuffer(OFF);
  memset(newGameBoard, 0, sizeof(newGameBoard));
  fdu.printBitmap(xOffs, yOffs, ON, xSize, ySize, s);

   // perturbInitialGameBoard();
}

/**
   Returns whether or not the specified cell is on.
   If the cell specified is outside the game board, returns false.
*/
bool isCellAlive(char row, char col) {
  if (row < 0 || col < 0 || row >= NUMROWS || col >= NUMCOLS) {
    return false;
  }
  return (fdu.getFrameBuffer(col,row));
}


/**
   Counts the number of active cells surrounding the specified cell.
   Cells outside the board are considered "off"
   Returns a number in the range of 0 <= n < 9
*/
uint8_t countNeighbors(uint8_t row, uint8_t col) {
  uint8_t count = 0;
  char rowDelta, colDelta;
  for ( rowDelta = -1; rowDelta <= 1; rowDelta++) {
    for ( colDelta = -1; colDelta <= 1; colDelta++) {
      // skip the center cell
      if (!(colDelta == 0 && rowDelta == 0)) {
        if (isCellAlive(rowDelta + row, colDelta + col)) {
          count++;
        }
      }
    }
  }

  return count;
}


/**
   Encodes the core rules of Conway's Game Of Life, and generates the next iteration of the board.
   Rules taken from wikipedia.
*/
void calculateNewGameBoard() {
  uint8_t row, col, numNeighbors;
  for ( row = 0; row < NUMROWS; row++) {
    for ( col = 0; col < NUMCOLS; col++) {
      numNeighbors = countNeighbors(row, col);
      if (fdu.getFrameBuffer(col,row) && numNeighbors < 2)
      {
        // Any live cell with fewer than two live neighbours dies, as if caused by under-population.
        setNewGameBoard(col, row, OFF); // newGameBoard[col][row] = false;
      }
      else if (fdu.getFrameBuffer(col,row) && (numNeighbors == 2 || numNeighbors == 3))
      {
        // Any live cell with two or three live neighbours lives on to the next generation.
         setNewGameBoard(col, row, ON); // newGameBoard[col][row] = true;
      }
      else if (fdu.getFrameBuffer(col,row) && numNeighbors > 3)
      {
        // Any live cell with more than three live neighbours dies, as if by overcrowding.
         setNewGameBoard(col, row, OFF); // newGameBoard[col][row] = false;
      }
      else if (!fdu.getFrameBuffer(col,row) && numNeighbors == 3)
      {
        // Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
         setNewGameBoard(col, row, ON); // newGameBoard[col][row] = true;
      }
      else
      {
        // All other cells will remain off
         setNewGameBoard(col, row, OFF); // newGameBoard[col][row] = false;
      }
    }
  }
}


/**
   Copies the data from the new game board into the current game board array
*/
void swapGameBoards() {
  uint8_t row, col, val;
  for ( row = 0; row < NUMROWS; row++) {
    for ( col = 0; col < NUMCOLS; col++) {
      val = getNewGameBoard(col, row);
      fdu.setFrameBuffer(col, row, val);
    }
  }
}

void showGameBoard() {
  //displayGameBoard();
  //delay(250);
  // Calculate the next iteration
  calculateNewGameBoard();
  swapGameBoards();
}

void  GameOfLife(void) {
  int iterations = 0;
  fdu.clearFrameBuffer(OFF);
  fdu.printString(1, 1, ON, SMALL, "Game of");
  fdu.printString(9, 9, ON, SMALL, "Life");
  fdu.updatePanel();
  delay(3000);
  iterations++;
  //initGameBoard(24, 6, 4, 4, "03060200");  // r-pentomino
  //initGameBoard(24, 6, 8, 8, "2008670000000000");   // acorn
  //initGameBoard(24, 4, 4, 8, "0705050005050700");   // 
  //initGameBoard(24, 2, 8, 4, "02C04700");   // die hard
  //initGameBoard(20, 8, 8, 6, "020b0a0820a0");   // infinity
  //initGameBoard(20, 8, 5, 5, "1D10030D15");   // infinity2
  initGameBoard(1, 8, 40, 1, "7FBE381FDF");   // infinity3
  //initGameBoard();
  fdu.updatePanel();
  delay(100);

  while (iterations < 60)
  {
    //clearAll(OFF);
    showGameBoard();
    //printFrameBuffer();
    fdu.updatePanel();
    iterations++;
    Serial.println(iterations, DEC);
  }

}
