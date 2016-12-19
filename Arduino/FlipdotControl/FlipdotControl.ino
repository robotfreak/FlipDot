////////////////////////////////////////////////////////////////////////////
// Flipdot_Firmware
// CC-BY-SA-NC    8/2016 C-Hack (Ralf@Surasto.de)
//  added support for 9x16 font   by robotfreak
//  added support for graphics    by robotfreak
//
// This main program runs on the Arduino and waits for commands via the RS232
// For simplicity the commands are handed over as an ascii string so that
// they can be easily created by scripts
//
// Command format
//   <Command>,<Color>,<x>,<y>,<size>,<....string....>\n
//
//   Commands:
//     C  Clear Screen
//     P  Print
//     H  Draw a horizontal line
//     V  Draw a vertical line
//     S  Set a pixel
//     U  Update Panel
//   Color:
//     B  Black
//     Y  Yellow
//   X,Y:
//     Required for all Print commands
//     Only Y is required for the horizontal line command "H"
//     Only X is required for the vertical line command "V"
//   size:
//     S SMALL
//     M MEDIUM
//     L LARGE
//     X EXTRALARGE
//   String:
//     Contains the characters to be printed
//   "\":
//     The command lines is terminated by the return character
//     It gets evaluated after reception of that character
//
////////////////////////////////////////////////////////////////////////////

#include "Flipdot.h"

int i, j;
int inByte;
String commandLine;

FlipDot flipdot(FD_COLUMS, FD_ROWS);

void setup() {

  Serial.begin(9600);
  flipdot.begin();
  flipdot.update();
  delay(1000);
}

void loop() {
  char c;
  int color;
  unsigned char cmd;
  int cmdPtr;
  int xVal, yVal;
  int xSiz, ySiz;
  char fontSize;
  int fsize;

  String xStr, yStr;
  String xSizStr, ySizStr;
  String outputString;

  if (Serial.available() > 0) {
    c = Serial.read();
    if (commandLine.length() < 100) {
      commandLine += c;
    }
    else {
      commandLine = "";
      Serial.print("?");
    }

    // ==== If command string is complete... =======
    if (c == '\\') {

      cmd = commandLine.charAt(0);
      if (commandLine.charAt(2) == 'B') color = 1; else color = 0;
      cmdPtr = 4;
      xStr = ""; yStr = "";
      while ((cmdPtr < commandLine.length()) && (commandLine.charAt(cmdPtr) != ',')) {
        xStr +=  (char)commandLine.charAt(cmdPtr);
        cmdPtr++;
        xVal = xStr.toInt();
      }

      cmdPtr++;
      while ((cmdPtr < commandLine.length()) && (commandLine.charAt(cmdPtr) != ',')) {
        yStr += (char)commandLine.charAt(cmdPtr);
        cmdPtr++;
        yVal = yStr.toInt();
      }

      if (cmd == 'B')  // Bitmap
      {
        xSizStr = ""; ySizStr = "";
        while ((cmdPtr < commandLine.length()) && (commandLine.charAt(cmdPtr) != ',')) {
          xSizStr +=  (char)commandLine.charAt(cmdPtr);
          cmdPtr++;
          xSiz = xSizStr.toInt();
        }

        cmdPtr++;
        while ((cmdPtr < commandLine.length()) && (commandLine.charAt(cmdPtr) != ',')) {
          ySizStr += (char)commandLine.charAt(cmdPtr);
          cmdPtr++;
          ySiz = ySizStr.toInt();
        }

      }
      else
      {
        cmdPtr++;
        fontSize = commandLine.charAt(cmdPtr);
        if (fontSize == 'S') fsize = SMALL;
        else if (fontSize == 'M') fsize = MEDIUM;
        else if (fontSize == 'L') fsize = LARGE;
        else fsize = XLARGE;
      }

      cmdPtr += 2;
      outputString = "";
      while ((cmdPtr < commandLine.length() - 1) && (outputString.length() < 100)) {
        outputString += (char)commandLine.charAt(cmdPtr);
        cmdPtr++;
      }

      commandLine = "";    // Reset command mode

      // ======= Debug only ===========
      Serial.println((char)cmd);
      Serial.println(color);
      Serial.println(xVal);
      Serial.println(yVal);
      if (cmd == 'B')
      {
        Serial.println(xSiz);
        Serial.println(ySiz);
      }
      else
        Serial.println(fontSize);
      Serial.println(outputString);

      // ======= Execute the respective command ========
      switch (cmd) {
        case 'C':  clearFrameBuffer(color); Serial.println("C"); updatePanel(); break;
        case 'G':  GameOfLife(); Serial.println("G"); break;
        case 'T':  printTest(yVal); Serial.println("T"); break;
        case 'S':  setPixel(xVal, yVal, color); break;
        case 'H':  hLine(yVal, color); updatePanel(); break;
        case 'V':  vLine(xVal, color); updatePanel(); break;
        case 'P':  printString(xVal, yVal, color, fsize, outputString); updatePanel(); break;
        case 'B':  printBitmap(xVal, yVal, color, xSiz, ySiz, outputString); updatePanel(); break;
        case 'U':  updatePanel(); Serial.println("U"); break;
      }
    }
  }
}


//===================================
// For debugging and testing only
//===================================
void printTest(int y) {

  clearFrameBuffer(OFF);
  i = printString(1, 0, ON, SMALL, "ABCDEFGHIJKLM");
  i = printString(1, 8, ON, SMALL, "NOPQRSTUVWXYZ");
  updatePanel();
  Serial.println("Small Font 6x8       ");
  delay(2000);

  clearFrameBuffer(OFF);
  i = printString(1, 0, ON, SMALL, "abcefghifklm");
  i = printString(1, 8, ON, SMALL, "noqrstuvwxyz");
  updatePanel();
  delay(2000);

  clearFrameBuffer(OFF);
  i = printString(1, 0, ON, MEDIUM, "ABCDEFGHIJKLM");
  i = printString(1, 8, ON, MEDIUM, "NOPQRSTUVWXYZ");
  updatePanel();
  Serial.println("Medium Font 8x8      ");
  delay(2000);

  clearFrameBuffer(OFF);
  i = printString(1, 0, ON, MEDIUM, "abcefghifklm");
  i = printString(1, 8, ON, MEDIUM, "noqrstuvwxyz");
  updatePanel();
  delay(2000);

  clearFrameBuffer(OFF);
  i = printString(2, 2, ON, LARGE, "ABCDEFGHIJKLM");
  Serial.println("Large Font 8x12      ");
  updatePanel();
  delay(2000);

  clearFrameBuffer(OFF);
  i = printString(2, 2, ON, LARGE, "abcefghifklm");
  updatePanel();
  delay(2000);

  clearFrameBuffer(OFF);
  i = printString(2, 0, ON, XLARGE, "ABCDEF");
  updatePanel();
  Serial.println("Extra Large Font 9x16 ");
  delay(2000);

  clearFrameBuffer(OFF);
  i = printString(2, 0, ON, XLARGE, "123456");
  updatePanel();
  delay(2000);

  clearFrameBuffer(OFF);
  //i =   printBitmap(0,0,ON,8,8,"1020408001020408");
  i = printBitmap(2, 0, ON, 4, 4, "09000906");
  i = printBitmap(2, 6, ON, 4, 4, "09000609");
  i = printBitmap(2, 12, ON, 4, 4, "09000f00");
  i = printBitmap(12, 0, ON, 8, 8, "0066660081423C00");
  i = printBitmap(12, 8, ON, 8, 8, "006666003C428100");
  i = printBitmap(22, 2, ON, 5, 5, "0A1F1F0E04");
  i = printBitmap(32, 2, ON, 8, 9, "FF81422418244281FF");
  //i = printBitmap(42, 0, ON, 16, 16, "1000010000100001111122224444888811112222444488881111222244448888");
  //i = printBitmap(60, 0, ON, 16, 16, "FFFF800180018001800180018001FFFF8001800180018001800180018001FFFF");
  //   vLine(0,ON);
  //   vLine(77,ON);
  //   hLine(0,ON);
  //   hLine(15,ON);
  updatePanel();
  Serial.println("Bitmap Grafik");
  delay(2000);
}
