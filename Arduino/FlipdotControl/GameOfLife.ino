#include <Arduino.h>
#include "flipdot.h"

#define NUMROWS 16
#define NUMCOLS 16

uint8_t gameBoard[NUMROWS][NUMCOLS];
uint8_t newGameBoard[NUMROWS][NUMCOLS];


void perturbInitialGameBoard() {
  int row, col;
  int numChanges, i;

  numChanges = random(20, 100);
  printf("numChanges: %d\n", numChanges);
  for ( i = 0; i < numChanges; i++) {
    row = random(0, NUMROWS);
    col = random(0, NUMCOLS);
    if (gameBoard[row][col] == 1) gameBoard[row][col] = 0;
    else gameBoard[row][col] = 1;
  }
}

void initGameBoard() {
  {
    uint8_t row, col;

    for (row = 0; row < NUMROWS; row++) {
      for (col = 0; col < NUMCOLS; col++) {
        gameBoard[row][col] = 0;
        newGameBoard[row][col] = 0;
      }
    }
  }
  //gameBoard[12][2] = 1;
  //gameBoard[12][3] = 1;
  //gameBoard[13][1] = 1;
  //gameBoard[13][2] = 1;
  //gameBoard[14][2] = 1;

  //perturbInitialGameBoard();

  // acorn

  // acorn

  gameBoard[7][9] = 1;
  gameBoard[8][9] = 1;
  gameBoard[8][7] = 1;
  gameBoard[10][8] = 1;
  gameBoard[11][9] = 1;
  gameBoard[12][9] = 1;
  gameBoard[13][9] = 1;

  perturbInitialGameBoard();

}
/**
   Loops over all game board positions, and briefly turns on any LEDs for "on" positions.
*/
void displayGameBoard() {
  uint8_t row, col;
  for (row = 0; row < NUMROWS; row++) {
    for (col = 0; col < NUMCOLS; col++) {
      if (gameBoard[row][col]) {
        setPixel(col, row, 1);
      }
    }
  }
}

/**
   Returns whether or not the specified cell is on.
   If the cell specified is outside the game board, returns false.
*/
bool isCellAlive(char row, char col) {
  if (row < 0 || col < 0 || row >= NUMROWS || col >= NUMCOLS) {
    return false;
  }
  return (gameBoard[row][col] == 1);
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
      if (gameBoard[row][col] && numNeighbors < 2) {
        // Any live cell with fewer than two live neighbours dies, as if caused by under-population.
        newGameBoard[row][col] = false;
        //setPixel(col, row, 0);
      } else if (gameBoard[row][col] && (numNeighbors == 2 || numNeighbors == 3)) {
        // Any live cell with two or three live neighbours lives on to the next generation.
        newGameBoard[row][col] = true;
        //setPixel(col, row, 1);
      } else if (gameBoard[row][col] && numNeighbors > 3) {
        // Any live cell with more than three live neighbours dies, as if by overcrowding.
        newGameBoard[row][col] = false;
        //setPixel(col, row, 0);
      } else if (!gameBoard[row][col] && numNeighbors == 3) {
        //} else if (gameBoard[row][col] && numNeighbors == 3) {
        // Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
        newGameBoard[row][col] = true;
        //setPixel(col, row, 1);
      } else {
        // All other cells will remain off
        newGameBoard[row][col] = false;
        //setPixel(col, row, 0);
      }
    }
  }
}


/**
   Copies the data from the new game board into the current game board array
*/
void swapGameBoards() {
  uint8_t row, col;
  for ( row = 0; row < NUMROWS; row++) {
    for ( col = 0; col < NUMCOLS; col++) {
      gameBoard[row][col] = newGameBoard[row][col];
      //gameBoard[row][col] = getFrameBuffer(col, row);
    }
  }
}

void showGameBoard() {
  displayGameBoard();
  //delay(250);
  // Calculate the next iteration
  calculateNewGameBoard();
  swapGameBoards();
}

void  GameOfLife(void) {
  int iterations = 0;
  clearAll(OFF);
  printString(1, 1, ON, MEDIUM, "Game of");
  printString(9, 9, ON, MEDIUM, "Life");
  //printFrameBuffer();
  updatePanel();
  delay(3000);
  iterations++;
  initGameBoard();

  while (iterations < 60)
  {
    clearAll(OFF);
    showGameBoard();
    //printFrameBuffer();
    updatePanel();
    iterations++;
    Serial.println(iterations, DEC);
  }

}


