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
// *    Game of Life functions                           *
// *******************************************************

#define NUMROWS Y_PIXELS
#define NUMCOLS X_SIZE

//bool gameBoard[NUMROWS][NUMCOLS];
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

//===========================================
// randomInt(int min_num, int max_num)
// calculates a random integer within the range
// min_num, max_num
//===========================================
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

//===========================================
// perturbInitialGameBoard()
// Do some random modification on the start bitmap
//===========================================
void perturbInitialGameBoard()
{
  int row, col, val;
  int numChanges, i;

  numChanges = randomInt(20, 100);
  //printf("numChanges: %d\n", numChanges);
  for (i = 0; i < numChanges; i++)
  {
    row = randomInt(0, NUMROWS);
    col = randomInt(0, NUMCOLS);
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

//===========================================
// isCellAlive(int row, int col)
// Returns whether or not the specified cell is on.
// If the cell specified is outside the game board, returns false.
//===========================================
bool isCellAlive(int row, int col)
{
  if (row < 0 || col < 0 || row >= NUMROWS || col >= NUMCOLS)
  {
    return false;
  }
  return (getFrameBuffer(col,row));   //gameBoard[row][col] == 1);
}

//===========================================
// countNeighbors(int row, int col)
// Counts the number of active cells surrounding the specified cell.
// Cells outside the board are considered "off"
// Returns a number in the range of 0 <= n < 9
//===========================================
int countNeighbors(int row, int col)
{
  int count = 0;
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

//===========================================
// calculateNewGameBoard()
// Encodes the core rules of Conway's Game Of Life, 
// and generates the next iteration of the board.
// Rules taken from wikipedia.
//===========================================
void calculateNewGameBoard()
{
  int row, col, numNeighbors;
  for (row = 0; row < NUMROWS; row++)
  {
    for (col = 0; col < NUMCOLS; col++)
    {
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

//===========================================
// swapGameBoards()
// Copies the data from the new game board 
// into the current game board array
//===========================================
void swapGameBoards()
{
  int row, col, val;
  for (row = 0; row < NUMROWS; row++)
  {
    for (col = 0; col < NUMCOLS; col++)
    {
      val = getNewGameBoard(col, row);
      setFrameBuffer(col, row, val);
    }
  }
}

//===========================================
// showGameBoard()
// calculates new board and update the framebuffer
//===========================================
void showGameBoard()
{
  //displayGameBoard();
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

  //initGameBoard(24, 6, 4, 4, "03060200");  // r-pentomino
  //initGameBoard(24, 6, 8, 8, "2008670000000000");   // acorn
  //initGameBoard(24, 4, 4, 8, "0705050005050700");   // 
  //initGameBoard(24, 2, 8, 4, "02C04700");   // die hard
  //initGameBoard(20, 8, 8, 6, "020b0a0820a0");   // infinity
  //initGameBoard(20, 8, 5, 5, "1D10030D15");   // infinity2
  initGameBoard(5, 8, 40, 1, "7FBE381FDF");   // infinity3
  //initGameBoard();
  printFrameBuffer();
  sleep(1);
  iterations++;

  while (iterations < 300)
  {
    showGameBoard();
    printFrameBuffer();
    printf("Iterations: %d     ", iterations++);
    usleep(100000);
  }
}
