#include <Arduino.h>
#include "Flipdot.h"

#define NUMROWS 16
#define NUMCOLS 50

//uint8_t gameBoard[NUMROWS][NUMCOLS];
uint8_t newGameBoard[NUMROWS][NUMCOLS];


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

void initGameBoard() {
  {
    uint8_t row, col;

   clearAll(OFF);
   for (row = 0; row < NUMROWS; row++) {
      for (col = 0; col < NUMCOLS; col++) {
        //gameBoard[row][col] = 0;
        newGameBoard[row][col] = 0;
      }
    }
  }
  // glider
  /*  
  setFrameBuffer(23,4,1);
  setFrameBuffer(24,5,1);
  setFrameBuffer(22,6,1);
  setFrameBuffer(23,6,1);
  setFrameBuffer(24,6,1);
*/
  /* r pentomino */
  setFrameBuffer(16,8,1);
  setFrameBuffer(17,7,1);
  setFrameBuffer(17,8,1);
  setFrameBuffer(17,9,1);
  setFrameBuffer(18,7,1);

  /* die hard 
  setFrameBuffer(16,8,1);
  setFrameBuffer(17,8,1);
  setFrameBuffer(17,9,1);
  setFrameBuffer(21,9,1);
  setFrameBuffer(22,8,1);
  setFrameBuffer(22,9,1);
  setFrameBuffer(23,9,1);
*/  
  /* acorn 
  setFrameBuffer(17,9,1);
  setFrameBuffer(18,7,1);
  setFrameBuffer(18,9,1);
  setFrameBuffer(20,8,1);
  setFrameBuffer(21,9,1);
  setFrameBuffer(22,9,1);
  setFrameBuffer(23,9,1);
*/
  perturbInitialGameBoard();

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
      if (getFrameBuffer(col,row) && numNeighbors < 2) {
        // Any live cell with fewer than two live neighbours dies, as if caused by under-population.
        newGameBoard[row][col] = false;
        //setPixel(col, row, 0);
      } else if (getFrameBuffer(col,row) && (numNeighbors == 2 || numNeighbors == 3)) {
        // Any live cell with two or three live neighbours lives on to the next generation.
        newGameBoard[row][col] = true;
        //setPixel(col, row, 1);
      } else if (getFrameBuffer(col,row) && numNeighbors > 3) {
        // Any live cell with more than three live neighbours dies, as if by overcrowding.
        newGameBoard[row][col] = false;
        //setPixel(col, row, 0);
      } else if (!getFrameBuffer(col,row) && numNeighbors == 3) {
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
      setFrameBuffer(col,row, newGameBoard[row][col]);
      //gameBoard[row][col] = getFrameBuffer(col, row);
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
  clearAll(OFF);
  printString(1, 1, ON, MEDIUM, "Game of");
  printString(9, 9, ON, MEDIUM, "Life");
  //printFrameBuffer();
  updatePanel();
  delay(3000);
  iterations++;
  initGameBoard();
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


