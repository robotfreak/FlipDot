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
//     B  Draw a Bitmap
//     P  Print Text
//     H  Draw a horizontal line
//     V  Draw a vertical line
//     S  Set a pixel
//     G  Game of Life
//     T  Tetris
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

#include <avr/pgmspace.h>
#include "Flipdot.h"
#include "FlipdotUtils.h"

int i, j;
int inByte;
String commandLine;

FlipDot flipdot(FD_COLUMS, FD_ROWS);
FlipDotUtils fdu(flipdot);

void setup() {

  Serial.begin(9600);
  flipdot.begin();
  flipdot.update();
  Serial.println("Init completed");
  //showHelp();
  //fdu.addFlipdot(flipdot);
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
    if (c == '\n') {

      cmd = commandLine.charAt(0);
      if (commandLine.charAt(2) == 'Y') color = 1; else color = 0;
      cmdPtr = 4;
      xStr = ""; yStr = "";
      xVal = 0; yVal = 0;
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
        xSiz = 0; ySiz = 0;
        cmdPtr++;
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
        cmdPtr++;
      }

      cmdPtr++;
      outputString = "";
      while ((cmdPtr < commandLine.length() - 1) && (outputString.length() < 100)) {
        outputString += (char)commandLine.charAt(cmdPtr);
        cmdPtr++;
      }

      commandLine = "";    // Reset command mode

      // ======= Debug only ===========
      Serial.println((char)cmd);
      Serial.print("Color: ");
      Serial.println(color);
      Serial.print("xVal: ");
      Serial.println(xVal);
      Serial.print("yVal: ");
      Serial.println(yVal);
      if (cmd == 'B')
      {
        Serial.print("xSiz: ");
        Serial.println(xSiz);
        Serial.print("ySiz: ");
        Serial.println(ySiz);
      }
      else
      {
        Serial.print("font: ");
        Serial.println(fontSize);
      }
      Serial.println(outputString);

      // ======= Execute the respective command ========
      switch (cmd) {
        case 'C':  fdu.clearFrameBuffer(color); Serial.println("C"); fdu.updatePanel(); break;
        case 'G':  GameOfLife(); Serial.println("G"); break;
        case 'T':  Tetris(); Serial.println("T"); break;
        case 'S':  fdu.setPixel(xVal, yVal, color); break;
        case 'H':  fdu.hLine(yVal, color); fdu.updatePanel(); Serial.println("H"); break;
        case 'V':  fdu.vLine(xVal, color); fdu.updatePanel(); Serial.println("V"); break;
        case 'P':  fdu.printString(xVal, yVal, color, fsize, outputString); fdu.updatePanel(); Serial.println("P");  break;
        case 'B':  fdu.printBitmap(xVal, yVal, color, xSiz, ySiz, outputString); fdu.updatePanel(); Serial.println("B"); break;
        case 'U':  fdu.updatePanel(); Serial.println("U"); break;
        case '?':  showHelp(); Serial.println("?"); break;
      }
    }
  }
}


//===================================
// For debugging and testing only
//===================================
void printTest(int y) {

  fdu.clearFrameBuffer(OFF);
  i = fdu.printString(1, 0, ON, XSMALL, "ABCDEFGHIJKLM");
  i = fdu.printString(1, 6, ON, XSMALL, "NOPQRSTUVWXYZ");
  i = fdu.printString(1,11, ON, XSMALL, "1234567890()[]");
  fdu.updatePanel();
  Serial.println("Extra Small Font 3x5  ");
  delay(2000);

  fdu.clearFrameBuffer(OFF);
  i = fdu.printString(1, 0, ON, SMALL, "ABCDEFGHIJKLM");
  i = fdu.printString(1, 8, ON, SMALL, "NOPQRSTUVWXYZ");
  fdu.updatePanel();
  Serial.println("Small Font 6x8       ");
  delay(2000);

  fdu.clearFrameBuffer(OFF);
  i = fdu.printString(1, 0, ON, SMALL, "abcdefghijklm");
  i = fdu.printString(1, 8, ON, SMALL, "nopqrstuvwxyz");
  fdu.updatePanel();
  delay(2000);

  fdu.clearFrameBuffer(OFF);
  i = fdu.printString(1, 0, ON, MEDIUM, "ABCDEFGHIJKLM");
  i = fdu.printString(1, 8, ON, MEDIUM, "NOPQRSTUVWXYZ");
  fdu.updatePanel();
  Serial.println("Medium Font 8x8      ");
  delay(2000);

  fdu.clearFrameBuffer(OFF);
  i = fdu.printString(1, 0, ON, MEDIUM, "abcdefghijklm");
  i = fdu.printString(1, 8, ON, MEDIUM, "nopqrstuvwxyz");
  fdu.updatePanel();
  delay(2000);

  fdu.clearFrameBuffer(OFF);
  i = fdu.printString(2, 2, ON, LARGE, "ABCDEFGHIJKLM");
  Serial.println("Large Font 8x12      ");
  fdu.updatePanel();
  delay(2000);

  fdu.clearFrameBuffer(OFF);
  i = fdu.printString(2, 2, ON, LARGE, "abcdefghijklm");
  fdu.updatePanel();
  delay(2000);

  fdu.clearFrameBuffer(OFF);
  i = fdu.printString(2, 0, ON, XLARGE, "ABCDEF");
  fdu.updatePanel();
  Serial.println("Extra Large Font 9x16 ");
  delay(2000);

  fdu.clearFrameBuffer(OFF);
  i = fdu.printString(2, 0, ON, XLARGE, "123456");
  fdu.updatePanel();
  delay(2000);

  fdu.clearFrameBuffer(OFF);
  //i =   fdu.printBitmap(0,0,ON,8,8,"1020408001020408");
  i = fdu.printBitmap(2, 0, ON, 4, 4, "09000906");
  i = fdu.printBitmap(2, 6, ON, 4, 4, "09000609");
  i = fdu.printBitmap(2, 12, ON, 4, 4, "09000f00");
  i = fdu.printBitmap(12, 0, ON, 8, 8, "0066660081423C00");
  i = fdu.printBitmap(12, 8, ON, 8, 8, "006666003C428100");
  i = fdu.printBitmap(22, 2, ON, 5, 5, "0A1F1F0E04");
  i = fdu.printBitmap(32, 2, ON, 8, 9, "FF81422418244281FF");
  //i = fdu.printBitmap(42, 0, ON, 16, 16, "1000010000100001111122224444888811112222444488881111222244448888");
  //i = fdu.printBitmap(60, 0, ON, 16, 16, "FFFF800180018001800180018001FFFF8001800180018001800180018001FFFF");
  //   fdu.vLine(0,ON);
  //   fdu.vLine(77,ON);
  //   fdu.hLine(0,ON);
  //   fdu.hLine(15,ON);
  fdu.updatePanel();
  Serial.println("Bitmap Grafik");
  delay(2000);
}

const char string_0[] PROGMEM = "Flipdot Demo v2.0"; 
const char string_1[] PROGMEM = "-----------------------";
const char string_2[] PROGMEM = "Command format";
const char string_3[] PROGMEM = "<Command>,<Color>,<x>,<y>,<size>,[<xsize>,<ysize>]<....string....>\\\n";
const char string_4[] PROGMEM = "Commands:";
const char string_5[] PROGMEM = "  C  Clear Screen";
const char string_6[] PROGMEM = "  B  Draw a Bitmap";
const char string_7[] PROGMEM = "  P  Print Text";
const char string_8[] PROGMEM = "  H  Draw a horizontal line";
const char string_9[] PROGMEM = "  V  Draw a vertical line";
const char string_10[] PROGMEM = "  S  Set a pixel";
const char string_11[] PROGMEM = "  G  Game of Life";
const char string_12[] PROGMEM = "Color:";
const char string_13[] PROGMEM = "  B  Black";
const char string_14[] PROGMEM = "  Y  Yellow";
const char string_15[] PROGMEM = "X,Y:";
const char string_16[] PROGMEM = "  Required for all Print commands";
const char string_17[] PROGMEM = "size:";
const char string_18[] PROGMEM = "  Not required for Bitmap commands";
const char string_19[] PROGMEM = "  S SMALL";
const char string_20[] PROGMEM = "  M MEDIUM";
const char string_21[] PROGMEM = "  L LARGE";
const char string_22[] PROGMEM = "  X EXTRALARGE";
const char string_23[] PROGMEM = "XSize,YSize:";
const char string_24[] PROGMEM = "  Required only for Bitmap commands";
const char string_25[] PROGMEM = "String:";
const char string_26[] PROGMEM = "  Contains the characters to be printed";
const char string_27[] PROGMEM = "\\n:";
const char string_28[] PROGMEM = "The command lines is terminated by the \\n character";


// Then set up a table to refer to your strings.

const char *const string_table[] PROGMEM = {string_0,  string_1,  string_2,  string_3,  string_4,  string_5,
string_6,  string_7,  string_8,  string_9,  string_10, string_11, string_12, string_13, string_14, string_15,
string_16, string_17, string_18, string_19, string_20, string_21, string_22, string_23, string_24, string_25,
string_26, string_27, string_28 };

char buffer[60];  // make sure this is large enough for the largest string it must hold

void showHelp(void)
{
  for (int i = 0; i < 29; i++) {
    strcpy_P(buffer, (char *)pgm_read_word(&(string_table[i])));  // Necessary casts and dereferencing, just copy.
    Serial.println(buffer);
  }
/*  
  Serial.println("Flipdot Demo v2.0");
  Serial.println("-----------------------");
  Serial.println("Command format");
  Serial.println("<Command>,<Color>,<x>,<y>,<size>,[<xsize>,<ysize>]<....string....>\\\n");
  //
  Serial.println("Commands:");
  Serial.println("  C  Clear Screen");
  Serial.println("  B  Draw a Bitmap");
  Serial.println("  L  RGB LED");
  Serial.println("  P  Print Text");
  Serial.println("  H  Draw a horizontal line");
  Serial.println("  V  Draw a vertical line");
  Serial.println("  S  Set a pixel");
  Serial.println("  U  Update Panel");
  Serial.println("Color:");
  Serial.println("  B  Black");
  Serial.println("  Y  Yellow");
  Serial.println("X,Y:");
  Serial.println("  Required for all Print commands");
  Serial.println("  Only Y is required for the horizontal line command \"H\"");
  Serial.println("  Only X is required for the vertical line command \"V\"");
  Serial.println("size:");
  Serial.println("  Not required for Bitmap commands");
  Serial.println("  S SMALL");
  Serial.println("  M MEDIUM");
  Serial.println("  L LARGE");
  Serial.println("  X EXTRALARGE");
  Serial.println("XSize,YSize:");
  Serial.println("  Required only for Bitmap commands");
  Serial.println("String:");
  Serial.println("  Contains the characters to be printed");
  Serial.println("\\n");
  Serial.println("  The command lines is terminated by the \n character");
  Serial.println("  It gets evaluated after reception of that character");
*/  
}
