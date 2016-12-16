/////////////////////////////////////////////////////////////////
//  Conway's Game of Life on the Flipdot display.
//  Game of Life based on:
//  http://brownsofa.org/blog/2010/12/30/arduino-8x8-game-of-life/
//  Flipdot Simulator based on:
//  CC-BY SA NC 2016 c-hack.de    ralf@surasto.de
//  by RobotFreak  webmaster@robotfreak.de
/////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "font6x8v.h"

//================== Constants ===============================
#define X_SIZE 40   // 128 column
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
unsigned char frameBuffer[X_SIZE][Y_SIZE];

//===========================================
// clearFrameBuffer(int color)
// Sets all bits to
//     Yellow if color = ON
//     Black if color  = OFF
//===========================================
void clearFrameBuffer(int color)
{
  int x, y;
  for (x = 0; x < X_SIZE; x++)
    for (y = 0; y < Y_SIZE; y++)
    {
      if (color == ON)
        frameBuffer[x][y] = 0xFF;
      else
        frameBuffer[x][y] = 0x00;
    }
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

void setCursorPos(int XPos, int YPos)
{
  printf("\033[%d;%dH", YPos + 1, XPos + 1);
}

void clearScreen()
{
  printf("\033[2J");
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

#define NUMROWS X_SIZE
#define NUMCOLS Y_PIXELS

bool gameBoard[NUMROWS][NUMCOLS];
bool newGameBoard[NUMROWS][NUMCOLS];

int randomInt(int min_num, int max_num)
{
  int result = 0, low_num = 0, hi_num = 0;
  if (min_num < max_num)
  {
    low_num = min_num;
    hi_num = max_num + 1; // this is done to include max_num in output.
  }
  else
  {
    low_num = max_num + 1; // this is done to include max_num in output.
    hi_num = min_num;
  }
  srand(time(NULL));
  result = (rand() % (hi_num - low_num)) + low_num;
  return result;
}

void perturbInitialGameBoard()
{
  int row, col;
  int numChanges, i;

  numChanges = randomInt(20, 100);
  //printf("numChanges: %d\n", numChanges);
  for (i = 0; i < numChanges; i++)
  {
    row = randomInt(0, NUMROWS);
    col = randomInt(0, NUMCOLS);
    if (gameBoard[row][col] == 1)
      gameBoard[row][col] = 0;
    else
      gameBoard[row][col] = 1;
  }
}

void initGameBoard()
{
  {
    uint8_t row, col;

    for (row = 0; row < NUMROWS; row++)
    {
      for (col = 0; col < NUMCOLS; col++)
      {
        gameBoard[row][col] = 0;
        newGameBoard[row][col] = 0;
      }
    }
  }

  // glider
  /*  
  gameBoard[23][4] = 1;
  gameBoard[24][5] = 1;
  gameBoard[22][6] = 1;
  gameBoard[23][6] = 1;
  gameBoard[24][6] = 1;
*/
  /* r pentomino */
  gameBoard[16][8] = 1;
  gameBoard[17][7] = 1;
  gameBoard[17][8] = 1;
  gameBoard[17][9] = 1;
  gameBoard[18][7] = 1;

  /* die hard 
  gameBoard[16][8] = 1;
  gameBoard[17][8] = 1;
  gameBoard[17][9] = 1;
  gameBoard[21][9] = 1;
  gameBoard[22][8] = 1;
  gameBoard[22][9] = 1;
  gameBoard[23][9] = 1;
*/  
  /* acorn 
  gameBoard[17][9] = 1;
  gameBoard[18][7] = 1;
  gameBoard[18][9] = 1;
  gameBoard[20][8] = 1;
  gameBoard[21][9] = 1;
  gameBoard[22][9] = 1;
  gameBoard[23][9] = 1;
*/
  perturbInitialGameBoard();
}
/**
 * Loops over all game board positions, and briefly turns on any LEDs for "on" positions.
 */
void displayGameBoard()
{
  uint8_t row, col;
  for (row = 0; row < NUMROWS; row++)
  {
    for (col = 0; col < NUMCOLS; col++)
    {
      if (gameBoard[row][col])
      {
        setFrameBuffer(row, col, 1);
      }
    }
  }
}

/**
 * Returns whether or not the specified cell is on.
 * If the cell specified is outside the game board, returns false.
 */
bool isCellAlive(char row, char col)
{
  if (row < 0 || col < 0 || row >= NUMROWS || col >= NUMCOLS)
  {
    return false;
  }
  return (gameBoard[row][col] == 1);
}

/**
 * Counts the number of active cells surrounding the specified cell.
 * Cells outside the board are considered "off"
 * Returns a number in the range of 0 <= n < 9
 */
uint8_t countNeighbors(uint8_t row, uint8_t col)
{
  uint8_t count = 0;
  char rowDelta, colDelta;
  for (rowDelta = -1; rowDelta <= 1; rowDelta++)
  {
    for (colDelta = -1; colDelta <= 1; colDelta++)
    {
      // skip the center cell
      if (!(colDelta == 0 && rowDelta == 0))
      {
        if (isCellAlive(rowDelta + row, colDelta + col))
        {
          count++;
        }
      }
    }
  }

  return count;
}

/**
 * Encodes the core rules of Conway's Game Of Life, and generates the next iteration of the board.
 * Rules taken from wikipedia.
 */
void calculateNewGameBoard()
{
  uint8_t row, col, numNeighbors;
  for (row = 0; row < NUMROWS; row++)
  {
    for (col = 0; col < NUMCOLS; col++)
    {
      numNeighbors = countNeighbors(row, col);
      if (gameBoard[row][col] && numNeighbors < 2)
      {
        // Any live cell with fewer than two live neighbours dies, as if caused by under-population.
        newGameBoard[row][col] = false;
      }
      else if (gameBoard[row][col] && (numNeighbors == 2 || numNeighbors == 3))
      {
        // Any live cell with two or three live neighbours lives on to the next generation.
        newGameBoard[row][col] = true;
      }
      else if (gameBoard[row][col] && numNeighbors > 3)
      {
        // Any live cell with more than three live neighbours dies, as if by overcrowding.
        newGameBoard[row][col] = false;
      }
      else if (!gameBoard[row][col] && numNeighbors == 3)
      {
        //} else if (gameBoard[row][col] && numNeighbors == 3) {
        // Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
        newGameBoard[row][col] = true;
      }
      else
      {
        // All other cells will remain off
        newGameBoard[row][col] = false;
      }
    }
  }
}

/**
 * Copies the data from the new game board into the current game board array
 */
void swapGameBoards()
{
  uint8_t row, col;
  for (row = 0; row < NUMROWS; row++)
  {
    for (col = 0; col < NUMCOLS; col++)
    {
      gameBoard[row][col] = newGameBoard[row][col];
    }
  }
}

void showGameBoard()
{
  displayGameBoard();
  calculateNewGameBoard();
  swapGameBoards();
}

//############################# Main ###############################
int main(int argc, char *argv[])
{

  int iterations = 0;

  clearScreen();
  clearFrameBuffer(OFF);
  printString(0, 1, ON, LARGE, "Game of");
  printString(8, 9, ON, LARGE, "Life");
  printFrameBuffer();
  sleep(3);

  initGameBoard();
  clearFrameBuffer(OFF);
  showGameBoard();
  printFrameBuffer();
  sleep(1);
  iterations++;

  while (iterations < 250)
  {
    clearFrameBuffer(OFF);
    showGameBoard();
    printFrameBuffer();
    printf("Iterations: %d     ", iterations++);
    usleep(100000);
  }
}
