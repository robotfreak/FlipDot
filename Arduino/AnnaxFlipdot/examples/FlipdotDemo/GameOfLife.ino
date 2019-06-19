#include <Arduino.h>
#include "Flipdot.h"

#define NUMROWS 16
#define NUMCOLS 25

//uint8_t gameBoard[NUMROWS][NUMCOLS];
uint8_t newGameBoard[NUMCOLS][NUMROWS/8];

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
    val = getFrameBuffer(col, row);    
    if (val)
      setFrameBuffer(col,row, 0);
    else
      setFrameBuffer(col,row, 1);
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
  clearFrameBuffer(OFF);
  memset(newGameBoard, 0, sizeof(newGameBoard));
  printBitmap(xOffs, yOffs, ON, xSize, ySize, s);

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
  return (getFrameBuffer(col,row));
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
      if (getFrameBuffer(col,row) && numNeighbors < 2)
      {
        // Any live cell with fewer than two live neighbours dies, as if caused by under-population.
        setNewGameBoard(col, row, OFF); // newGameBoard[col][row] = false;
      }
      else if (getFrameBuffer(col,row) && (numNeighbors == 2 || numNeighbors == 3))
      {
        // Any live cell with two or three live neighbours lives on to the next generation.
         setNewGameBoard(col, row, ON); // newGameBoard[col][row] = true;
      }
      else if (getFrameBuffer(col,row) && numNeighbors > 3)
      {
        // Any live cell with more than three live neighbours dies, as if by overcrowding.
         setNewGameBoard(col, row, OFF); // newGameBoard[col][row] = false;
      }
      else if (!getFrameBuffer(col,row) && numNeighbors == 3)
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
      setFrameBuffer(col, row, val);
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
  clearFrameBuffer(OFF);
  printString(1, 1, ON, SMALL, "Game of");
  printString(9, 9, ON, SMALL, "Life");
  updatePanel();
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
  updatePanel();
  delay(1000);

  while (iterations < 60)
  {
    //clearAll(OFF);
    showGameBoard();
    //printFrameBuffer();
    updatePanel();
    iterations++;
    Serial.println(iterations, DEC);
  }

}


