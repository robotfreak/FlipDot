
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
////////////////////////////////////////////////////////////////////////////
#include "Flipdot.h"

const byte LED_RED = 3;
const byte LED_GREEN = 6;
const byte LED_BLUE = 7;  // no PWM pin

const byte STOP_PIN = 2;

#define TXT_BUF_SIZ 150
#define SCROLL_TIME 10

char textBuf[TXT_BUF_SIZ];
int textBufLen;
bool scroll = false;
int scrollDelay = SCROLL_TIME;

int i, j;
int inByte;
String commandLine;
String outputString;

int fdState = 0;
int fdMode = 0;
int r, g, b;

FlipDot flipdot(FD_COLUMS, FD_ROWS);

unsigned long current_time;
unsigned long last_time;
boolean timeOut = false;

void setup() {

  Serial.begin(115200);
  Serial.println("FlipdotTwitter v0.1");

  flipdot.begin();
  updatePanel();
  delay(1000);
  setLedColor(0xFF, 0, 0);
  delay(1000);
  setLedColor(0, 0, 0xFF);
  delay(1000);

  r = 0;
  g = 0xFF;
  b = 0;
  setLedColor(r, g, b);

  clearFrameBuffer(OFF);
  i = printString(5, 0, ON, XLARGE, "IN-BERLIN");
  updatePanel();
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

  // ======= Debug only ===========
  if (cmd == 'L')
  {
    Serial.println("ok");
    Serial.print("Red: ");
    Serial.println(r);
    Serial.print("Green: ");
    Serial.println(g);
    Serial.print("Blue: ");
    Serial.println(b);
  }
  else
  {
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
  }

  fdMode = 0;
  fdState = 0;
  // ======= Execute the respective command ========
  switch (cmd) {
    case 'C':  clearFrameBuffer(color); Serial.println("C"); updatePanel(); break;
    case 'L':  setLedColor(r, g, b); Serial.println("Led"); break;
    case 'S':  setPixel(xVal, yVal, color); break;
    case 'H':  hLine(yVal, color); updatePanel(); Serial.println("H"); break;
    case 'V':  vLine(xVal, color); updatePanel(); Serial.println("V"); break;
    case 'P':  if (outputString.length() > 19) scrollText(outputString); else printString(xVal, yVal, color, fsize, outputString); updatePanel(); Serial.println("P");  break;
    case 'B':  printBitmap(xVal, yVal, color, xSiz, ySiz, outputString); updatePanel(); Serial.println("B"); break;
    case 'U':  updatePanel(); Serial.println("U"); break;
    case 'f':  fdMode = 1; break;
    case 'n':  fdMode = 2; break;
    case 't':  fdMode = 3; break;
    case 'd':  fdMode = 4; break;
      // case 'h':  showHelp(); break;
  }
}

void loop() {

  if (true == checkForCommand())
    execCommand();

  //printNews();
}

void scrollText(String str)
{
  int ofs = 0;
  textBufLen = str.length();
  if (textBufLen < TXT_BUF_SIZ)
  {
    strcpy(textBuf, str.c_str());
    printString(0, 0, ON, XSMALL, &textBuf[0] );
    ofs += 19;
    textBufLen -= 19;
    printString(0, 8, ON, XSMALL, &textBuf[ofs] );
    ofs += 19;
    textBufLen -= 19;
    updatePanel();
    delay(10000);
    while (textBufLen > 0)
    {
      scrollFrameBuffer();
      printString(0, 8, ON, XSMALL, &textBuf[ofs] );
      ofs += 19;
      textBufLen -= 19;
      updatePanel();
      delay(10000);
    }
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
    clearFrameBuffer(OFF);
    switch (fdState)
    {
      case 0:
        Serial.println("In-Berlin");
        i = printString(5, 0, ON, XLARGE, "IN-BERLIN");
        fdState++;
        break;
      case 1:
        Serial.println("eLab");
        i = printString(5, 0, ON, XSMALL, "Di 19-22");
        i = printString(5, 8, ON, XSMALL, "Fr 19-01");
        i = printString(60, 0, ON, XLARGE, "ELAB");
        fdState++;
        break;
      case 2:
        Serial.println("BeLUG");
        i = printString(5, 5, ON, XSMALL, "Mi 18-21");
        i = printString(58, 0, ON, XLARGE, "BELUG");
        fdState = 0;
        break;
      default:
        fdState = 0;
        break;

    }
    updatePanel();
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
    clearFrameBuffer(OFF);
    switch (fdState)
    {
      case 0:
        i = printString(1, 0, ON, XSMALL, "ABCDEFGHIJKLM");
        i = printString(1, 6, ON, XSMALL, "NOPQRSTUVWXYZ");
        i = printString(1, 11, ON, XSMALL, "1234567890()[]");
        Serial.println("Extra Small Font 3x5  ");
        fdState++;
        break;
      case 1:
        i = printString(1, 0, ON, SMALL, "ABCDEFGHIJKLM");
        i = printString(1, 8, ON, SMALL, "NOPQRSTUVWXYZ");
        Serial.println("Small Font 6x8       ");
        fdState++;
        break;
      case 2:
        i = printString(1, 0, ON, MEDIUM, "ABCDEFGHIJKLM");
        i = printString(1, 8, ON, MEDIUM, "NOPQRSTUVWXYZ");
        Serial.println("Medium Font 8x8      ");
        fdState++;
        break;
      case 3:
        i = printString(2, 2, ON, LARGE, "ABCDEFGHIJKLM");
        Serial.println("Large Font 8x12      ");
        fdState++;
        break;
      case 4:
        i = printString(2, 0, ON, XLARGE, "ABCDEF");
        Serial.println("Extra Large Font 9x16 ");
        fdState++;
        break;
      case 5:
        i = printBitmap(2, 0, ON, 4, 4, "09000906");
        i = printBitmap(2, 6, ON, 4, 4, "09000609");
        i = printBitmap(2, 12, ON, 4, 4, "09000f00");
        i = printBitmap(12, 0, ON, 8, 8, "0066660081423C00");
        i = printBitmap(12, 8, ON, 8, 8, "006666003C428100");
        i = printBitmap(22, 2, ON, 5, 5, "0A1F1F0E04");
        i = printBitmap(32, 2, ON, 8, 9, "FF81422418244281FF");
        Serial.println("Bitmap Grafik");
        fdState++;
        break;
      default:
        fdState = 0;
        break;
    }
    updatePanel();
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
        clearFrameBuffer(OFF);
        fdState = 1;
        break;
      case 1:
        clearFrameBuffer(ON);
        fdState = 0;
        break;
      default:
        fdState = 0;
        break;

    }
    updatePanel();
  }
}

void setLedColor(uint16_t red, uint16_t green, uint16_t blue)
{
  analogWrite(LED_RED, red);
  analogWrite(LED_GREEN, green);
  analogWrite(LED_BLUE, blue);
}


