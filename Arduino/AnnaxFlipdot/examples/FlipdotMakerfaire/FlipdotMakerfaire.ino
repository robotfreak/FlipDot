
////////////////////////////////////////////////////////////////////////////
// Flipdot_Firmware
// CC-BY-SA-NC    8/2016 C-Hack (Ralf@Surasto.de)
//  added support for 9x16 font, bitmap graphics   by robotfreak
//  added support for Annax/AEG flipdot panels     by robotfreak
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
//   "\n":
//     The command lines is terminated by the new line character
//     It gets evaluated after reception of that character
//
//   This sketch uses the following external libraries:
//   MemoryFree: https://github.com/McNeight/MemoryFree
//
////////////////////////////////////////////////////////////////////////////
#include <Wire.h>

#include "Flipdot.h"
#include "FlipdotUtils.h"

int i, j;
int inByte;
String commandLine;
//unsigned long previousMillis = 0;        // will store last time from update
int fdState = 0;
int fdMode = 0;
int r, g, b;

FlipDot flipdot(FD_COLUMS, FD_ROWS);
FlipDotUtils fdu(flipdot);

void setup() {

  Serial.begin(115200);
  Serial.println("FlipdotControl v1.1");

  flipdot.begin();
  fdu.updatePanel();

  fdu.clearFrameBuffer(OFF);
  i = fdu.printString(2, 0, ON, LARGE, "ELAB-SPACE");
  fdu.updatePanel();
}

boolean checkForCommand(void) {
  char c;

  boolean ret = false;
  if (Serial.available() > 0) {
    c = Serial.read();
    if (commandLine.length() < 100) {
      if (c != '\r')
        commandLine += c;
    }
    else {
      commandLine = "";
      Serial.print("?");
    }

    // ==== If command string is complete... =======
    if (c == '\n') {
      ret = true;
    }
  }
  return ret;
}

void execCommand() {

  int color;
  unsigned char cmd;
  int cmdPtr;
  int xVal, yVal;
  int xSiz, ySiz;
  char fontSize;
  int fsize;

  String str;
  String outputString;

  //Serial.println(commandLine);

  cmd = commandLine.charAt(0);
  if (cmd == 'L')  // RGB LED
  {
    cmdPtr = 2;
    r = g = b = 0;
    str = "";
    while ((cmdPtr < commandLine.length()) && (commandLine.charAt(cmdPtr) != ',')) {
      str +=  (char)commandLine.charAt(cmdPtr);
      cmdPtr++;
    }
    cmdPtr++;
    r = str.toInt();
    str = "";
    while ((cmdPtr < commandLine.length()) && (commandLine.charAt(cmdPtr) != ',')) {
      str +=  (char)commandLine.charAt(cmdPtr);
      cmdPtr++;
    }
    cmdPtr++;
    g = str.toInt();
    str = "";
    while ((cmdPtr < commandLine.length()) && (commandLine.charAt(cmdPtr) != ',')) {
      str +=  (char)commandLine.charAt(cmdPtr);
      cmdPtr++;
    }
    cmdPtr++;
    b = str.toInt();
  }
  else {
    if (commandLine.charAt(2) == 'Y') color = 1; else color = 0;
    cmdPtr = 4;
    str = "";
    xVal = 0; yVal = 0;
    while ((cmdPtr < commandLine.length()) && (commandLine.charAt(cmdPtr) != ',')) {
      str +=  (char)commandLine.charAt(cmdPtr);
      cmdPtr++;
    }
    xVal = str.toInt();

    str = "";
    cmdPtr++;
    while ((cmdPtr < commandLine.length()) && (commandLine.charAt(cmdPtr) != ',')) {
      str += (char)commandLine.charAt(cmdPtr);
      cmdPtr++;
    }
    yVal = str.toInt();

    if (cmd == 'B')  // Bitmap
    {
      str = "";
      xSiz = 0; ySiz = 0;
      cmdPtr++;
      while ((cmdPtr < commandLine.length()) && (commandLine.charAt(cmdPtr) != ',')) {
        str +=  (char)commandLine.charAt(cmdPtr);
        cmdPtr++;
      }
      xSiz = str.toInt();

      str = "";
      cmdPtr++;
      while ((cmdPtr < commandLine.length()) && (commandLine.charAt(cmdPtr) != ',')) {
        str += (char)commandLine.charAt(cmdPtr);
        cmdPtr++;
      }
      ySiz = str.toInt();

    }
    else
    {
      cmdPtr++;
      fontSize = commandLine.charAt(cmdPtr);
      if (fontSize == 'E') fsize = XSMALL;
      else if (fontSize == 'S') fsize = SMALL;
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
  }
  commandLine = "";    // Reset command mode

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

  fdMode = 0;
  fdState = 0;
  // ======= Execute the respective command ========
  switch (cmd) {
    case 'C':  fdu.clearFrameBuffer(color); Serial.println("C"); fdu.updatePanel(); break;
    case 'S':  fdu.setPixel(xVal, yVal, color); break;
    case 'H':  fdu.hLine(yVal, color); fdu.updatePanel(); Serial.println("H"); break;
    case 'V':  fdu.vLine(xVal, color); fdu.updatePanel(); Serial.println("V"); break;
    case 'P':  fdu.printString(xVal, yVal, color, fsize, outputString); fdu.updatePanel(); Serial.println("P");  break;
    case 'B':  fdu.printBitmap(xVal, yVal, color, xSiz, ySiz, outputString); fdu.updatePanel(); Serial.println("B"); break;
    case 'U':  fdu.updatePanel(); Serial.println("U"); break;
    case 'f':  fdMode = 1; break;
    case 'n':  fdMode = 2; break;
    case 't':  fdMode = 3; break;
    case 'd':  fdMode = 4; break;
      // case 'h':  showHelp(); break;
  }
}

void loop() {
  boolean btn;

  if (true == checkForCommand())
    execCommand();

  printNews();

  switch (fdMode) {
    case 1:
      flipTest();
      break;
    case 2:
      printNews();
      break;
    case 3:
      printTest();
      break;
    case 4:
      //printDateTime();
      break;
    default:
      break;
  }

}

//Print an integer in "00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintI00(int val)
{
  if (val < 10) Serial.print('0');
  Serial.print(val, DEC);
  return;
}

//Print an integer in ":00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintDigits(int val)
{
  Serial.print(':');
  if (val < 10) Serial.print('0');
  Serial.print(val, DEC);
}


void print2digits(int number) {
  if (number == 255) {
    Serial.print("??");
  }
  else {
    if (number >= 0 && number < 10) {
      Serial.write('0');
    }
    Serial.print(number);
  }
}

//===================================
// For debugging and testing only
//===================================
void printNews() {
  static unsigned long previousMillis = 0;        // will store last time from update

  unsigned long currentMillis = millis();
  String str;

  if (currentMillis - previousMillis >= 60000) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    fdu.clearFrameBuffer(OFF);
    switch (fdState)
    {
      case 0:
        Serial.println("eLab");
        i = fdu.printString(5, 0, ON, XSMALL, "Di 18-23");
        i = fdu.printString(5, 8, ON, XSMALL, "Fr 18-..");
        i = fdu.printString(60, 0, ON, XLARGE, "ELAB");
        fdState++;
        break;
      case 1:
        Serial.println("url");
        i = fdu.printString(5, 4, ON, XSMALL, "elab.in-berlin.de");
        fdState++;
        break;
      case 2:
        Serial.println("projekte");
        i = fdu.printString(5, 0, ON, XSMALL, "Arduino, Robotik");
        i = fdu.printString(7, 8, ON, XSMALL, "Raspberry, ESP...");
        fdState++;
        break;
      case 3:
        Serial.println("komm");
        i = fdu.printString(15, 4, ON, XSMALL, "Komm zu uns!");
        fdState++;
        break;
      case 4:
        Serial.println("eLab");
        i = fdu.printString(5, 0, ON, XSMALL, "Dem freundlichen");
        i = fdu.printString(20, 8, ON, XSMALL, "Makerspace");
        fdState++;
        break;
      case 5:
        Serial.println("eLab");
        i = fdu.printString(2, 0, ON, LARGE, "ELAB-SPACE");
        fdState = 0;
        break;
      default:
        fdState = 0;
        break;

    }
    fdu.updatePanel();
  }

}

//===================================
// For debugging and testing only
//===================================
void printTest() {
  static unsigned long previousMillis = 0;        // will store last time from update
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 10000) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    fdu.clearFrameBuffer(OFF);
    switch (fdState)
    {
      case 0:
        i = fdu.printString(1, 0, ON, XSMALL, "ABCDEFGHIJKLM");
        i = fdu.printString(1, 6, ON, XSMALL, "NOPQRSTUVWXYZ");
        i = fdu.printString(1, 11, ON, XSMALL, "1234567890()[]");
        Serial.println("Extra Small Font 3x5  ");
        fdState++;
        break;
      case 1:
        i = fdu.printString(1, 0, ON, SMALL, "ABCDEFGHIJKLM");
        i = fdu.printString(1, 8, ON, SMALL, "NOPQRSTUVWXYZ");
        Serial.println("Small Font 6x8       ");
        fdState++;
        break;
      case 2:
        i = fdu.printString(1, 0, ON, MEDIUM, "ABCDEFGHIJKLM");
        i = fdu.printString(1, 8, ON, MEDIUM, "NOPQRSTUVWXYZ");
        Serial.println("Medium Font 8x8      ");
        fdState++;
        break;
      case 3:
        i = fdu.printString(2, 2, ON, LARGE, "ABCDEFGHIJKLM");
        Serial.println("Large Font 8x12      ");
        fdState++;
        break;
      case 4:
        i = fdu.printString(2, 0, ON, XLARGE, "ABCDEF");
        Serial.println("Extra Large Font 9x16 ");
        fdState++;
        break;
      case 5:
        i = fdu.printBitmap(2, 0, ON, 4, 4, "09000906");
        i = fdu.printBitmap(2, 6, ON, 4, 4, "09000609");
        i = fdu.printBitmap(2, 12, ON, 4, 4, "09000f00");
        i = fdu.printBitmap(12, 0, ON, 8, 8, "0066660081423C00");
        i = fdu.printBitmap(12, 8, ON, 8, 8, "006666003C428100");
        i = fdu.printBitmap(22, 2, ON, 5, 5, "0A1F1F0E04");
        i = fdu.printBitmap(32, 2, ON, 8, 9, "FF81422418244281FF");
        Serial.println("Bitmap Grafik");
        fdState++;
        break;
      default:
        fdState = 0;
        break;
    }
    fdu.updatePanel();
  }
}

//===================================
// For debugging and testing only
//===================================
void flipTest(void) {
  static unsigned long previousMillis = 0;        // will store last time from update

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 10000) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    switch (fdState)
    {
      case 0:
        fdu.clearFrameBuffer(OFF);
        fdState = 1;
        break;
      case 1:
        fdu.clearFrameBuffer(ON);
        fdState = 0;
        break;
      default:
        fdState = 0;
        break;

    }
    fdu.updatePanel();
  }
}
