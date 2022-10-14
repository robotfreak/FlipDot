
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
//     L  Set LED color (r,g,b)
//     U  Update Panel
//   Color:
//     B  Black
//     Y  Yellow
//     r  Red
//     g  Green
//     b  Blue
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
#include <SoftwareSerial.h>
#include "Flipdot.h"
#include "FlipdotUtils.h"

#define TXT_BUF_SIZ 150
#define SCROLL_TIME 10

const byte LED_RED = 3;
const byte LED_GREEN = 6;
const byte LED_BLUE = 7;  // no PWM pin

char textBuf[TXT_BUF_SIZ];
int textBufLen;
bool scroll = false;
int scrollDelay = SCROLL_TIME;

int i, j;
int inByte;
String commandLine;
String outputString;
char cmdlineBA[128];
int cmdPt = 0;
int cmdSiz = 0;

int fdState = 0;
int fdMode = 0;

int r, g, b;

SoftwareSerial mySerial(A5, A4); // RX, TX

FlipDot flipdot(FD_COLUMS, FD_ROWS);
FlipDotUtils fdu(flipdot);
/**
   Made with Marlin Bitmap Converter
   https://marlinfw.org/tools/u8glib/converter.html

   This bitmap from the file 'bC-Logo-Pixel-115x16-grSchw-2.png'
*/
#pragma once

#define LHVRE_BMPWIDTH  104

const unsigned char bitmap_lhvre[] PROGMEM = {
  B00000000, B00000000, B00000000, B00000000, B00000000, B10000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00010000, B10000100, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00001000, B10001000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000011, B11100000, B00001111, B11100000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B11000000, B00000000, B00001100, B00000000, B00000110, B00110000, B00011111, B11110000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B11000000, B00000000, B00001100, B11000000, B11101100, B00011011, B10111100, B01111000, B00000000, B00000000, B00100000, B00000000, B00000000,
  B11000000, B00000000, B00001100, B11000000, B00001000, B00001000, B01110000, B00011000, B00000000, B00000000, B01100000, B00000000, B00000000,
  B11000000, B01110000, B11101100, B00011111, B00001000, B00001000, B01110000, B00000001, B11001110, B01111100, B11111001, B11100001, B11001111,
  B11111100, B11111001, B11101101, B11011111, B10001000, B00001000, B01100000, B00000011, B11011111, B01111110, B11111011, B11110011, B11011111,
  B11111110, B11011001, B10001101, B11011001, B10001100, B00011000, B01100000, B00000011, B00011011, B00000110, B01100111, B00111011, B00011000,
  B11000110, B11111001, B10001100, B11011001, B10000110, B00110000, B01110000, B00000011, B00011111, B00111110, B01100110, B00011011, B00011110,
  B11000110, B11110001, B10001100, B11011001, B10000010, B00100000, B01110000, B00011011, B00011110, B01111110, B01100110, B00011011, B00001111,
  B11000110, B11000001, B10001100, B11011001, B10000011, B11100000, B00111000, B01111011, B00011000, B01100110, B01100111, B00111011, B00000011,
  B11111110, B11111001, B10001110, B11011001, B10000011, B11100000, B00011111, B11110011, B00011111, B01111110, B01111011, B11110011, B00011111,
  B01111100, B01111001, B10000110, B11011001, B10000001, B11000000, B00001111, B11000011, B00001111, B00111110, B00111001, B11100011, B00011110
};

void setup() {

  Serial.begin(115200);
  Serial.println("Flipdot berlinCreators v0.3");
  mySerial.begin(9600);

  r = 0;
  g = 0xff;
  b = 0;
  setLedColor(r, g, b);

  flipdot.begin();
  fdu.setSerialDebug(true);
  fdu.updatePanel();
  fdu.clearFrameBuffer(OFF);
  printNews(4);
}

boolean checkForCommand(void) {
  char c = 0;

  boolean ret = false;
  if (mySerial.available() > 0) {
    c = mySerial.read();
  }
  else if (Serial.available() > 0) {
    c = Serial.read();
  }
  else {
    return ret;
  }
  if (c) {
    Serial.write(c);
    if (cmdPt < 128 ) { //commandLine.length() < 100) {
      if (c != '\r' || c != '\n') {
        commandLine += c;
        cmdlineBA[cmdPt++] = c;
        cmdSiz++;
      }
    }
    else {
      commandLine = "";
      cmdlineBA[0] = 0;
      cmdPt = 0;
      cmdSiz = 0;
      Serial.print("Cmd too long");
    }

    // ==== If command string is complete... =======
    if (c == '\n' || c == '\r') {
      if (cmdlineBA[0]) { //commandLine.charAt(0) != '>') {
        commandLine += '\n';
        cmdlineBA[cmdPt++] = '\n';
        cmdlineBA[cmdPt++] = 0;
        cmdSiz++;
        cmdPt = 0;
        Serial.print("Cmd: ");
        Serial.println(cmdlineBA);
        Serial.println("Cmd complete");
        ret = true;
      }
      else {
        commandLine = "";
        cmdlineBA[0] = 0;
        cmdPt = 0;
        cmdSiz = 0;
      }
    }
  }
  return ret;
}

void execCommand() {

  int color;
  unsigned char cmd = 0;
  int cmdPtr;
  int xVal, yVal = 0;
  int xSiz, ySiz = 0;
  char fontSize = 0;
  char cVal = 0;
  int fsize;
  char * delimiter = ",";
  char * token;
  int i = 0;
  int strLen;

  Serial.print("csiz: ");
  Serial.println(cmdSiz);

  cmd = cmdlineBA[0]; //commandLine.charAt(0); //ba[0];
  if (cmd == '>') {
    commandLine = "";    // Reset command mode
    cmdlineBA[0] = 0;
    cmdPt = 0;
    //return;
  }
  else {
    token = strtok(cmdlineBA, delimiter);
    if (token != NULL) {
      cmd = token[0];
      token = strtok(NULL, delimiter);
      if (token != NULL) {
        cVal = token[0];
        token = strtok(NULL, delimiter);
        if (token != NULL) {
          xVal = atoi(token);
          token = strtok(NULL, delimiter);
          if (token != NULL) {
            yVal = atoi(token);
          }
        }
      }
    }
    else return;
    Serial.print("cmd: ");
    Serial.println(cmd);

    //cVal = cmdlineBA[2]; //commandLine.charAt(2); //ba[2];
    Serial.print("col: ");
    Serial.println(cVal);
    switch (cVal) {
      case 'Y': color = 1; Serial.println("Yellow"); break;
      case 'r': color = 2; Serial.println("Red"); break;
      case 'g': color = 3; Serial.println("Green"); break;
      case 'b': color = 4; Serial.println("Blue"); break;
      default: color = 0;  Serial.println("Black"); break;
    }

    if (cmd == 'B')  // Bitmap
    {
      //str = "";
      token = strtok(NULL, delimiter);
      if (token != NULL) {
        xSiz = atoi(token);
        token = strtok(NULL, delimiter);
        if (token != NULL) {
          ySiz = atoi(token);
        }
      }
    }
    else if (cmd == 'P')
    {
      token = strtok(NULL, delimiter);
      if (token != NULL) {
        fontSize = token[0];

        if (fontSize == 'E') fsize = XSMALL;
        else if (fontSize == 'S') fsize = SMALL;
        else if (fontSize == 'M') fsize = MEDIUM;
        else if (fontSize == 'L') fsize = LARGE;
        else fsize = XLARGE;
      }
    }

    commandLine = "";    // Reset command mode
    cmdlineBA[0] = 0;
    cmdPt = 0;
    cmdSiz = 0;

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
    else if (cmd == 'P')
    {
      Serial.print("font: ");
      Serial.println(fontSize);
    }

    fdMode = 0;
    fdState = 0;
    // ======= Execute the respective command ========
    switch (cmd) {
      case 'C':  fdu.clearFrameBuffer(color); Serial.println("C"); fdu.updatePanel(); break;
      case 'S':  fdu.setPixel(xVal, yVal, color); fdu.updatePanel(); break;
      case 'H':  fdu.hLine(yVal, color); fdu.updatePanel(); Serial.println("H"); break;
      case 'V':  fdu.vLine(xVal, color); fdu.updatePanel(); Serial.println("V"); break;
      //case 'P':  if (outputString.length() > 19) scrollText(outputString); else fdu.printString(xVal, yVal, color, fsize, ba, sizeof(outputString)); fdu.updatePanel(); Serial.println("P");  break;
      case 'B':  fdu.printBitmap(xVal, yVal, color, xSiz, ySiz, outputString); fdu.updatePanel(); Serial.println("B"); break;
      case 'L':  if (color == 2) setLedColor(0xff, 0, 0);
        else if (color == 3) setLedColor(0, 0xff, 0);
        else if (color == 4) setLedColor(0, 0, 0xff);
        else setLedColor(0, 0, 0);
        break;
      case 'U':  fdu.updatePanel(); Serial.println("U"); break;
      case 'f':  fdMode = 1; break;
      case 'n':  printNews(xVal); break;
      case 't':  fdMode = 3; break;
      case 'd':  fdMode = 4; break;
        // case 'h':  showHelp(); break;
    }
  }
}

void loop() {

  if (true == checkForCommand())
    execCommand();
}

void setLedColor(uint16_t red, uint16_t green, uint16_t blue)
{
  analogWrite(LED_RED, red);
  analogWrite(LED_GREEN, green);
  analogWrite(LED_BLUE, blue);
}

void scrollText(String str)
{
  int ofs = 0;
  textBufLen = str.length();
  if (textBufLen < TXT_BUF_SIZ)
  {
    strcpy(textBuf, str.c_str());
    fdu.printString(0, 0, ON, XSMALL, &textBuf[0], textBufLen);
    ofs += 19;
    textBufLen -= 19;
    fdu.printString(0, 8, ON, XSMALL, &textBuf[ofs], textBufLen);
    ofs += 19;
    textBufLen -= 19;
    fdu.updatePanel();
    delay(10000);
    while (textBufLen > 0)
    {
      fdu.scrollFrameBuffer();
      fdu.printString(0, 8, ON, XSMALL, &textBuf[ofs], textBufLen);
      ofs += 19;
      textBufLen -= 19;
      fdu.updatePanel();
      delay(10000);
    }
  }
}

//===================================
// For debugging and testing only
//===================================
void printNews(int state) {
  static unsigned long previousMillis = 0;        // will store last time from update
  bool trigger = false;
  unsigned long currentMillis = millis();
  String str;

  if (state >= 0) {
    fdState = state;
    trigger = true;
  }
  else if (currentMillis - previousMillis >= 60000) {
    previousMillis = currentMillis;
    trigger = true;
  }
  if (trigger) {
    fdu.clearFrameBuffer(OFF);
    switch (fdState)
    {
      case 0:
        Serial.println("In-Berlin");
        i = fdu.printString(5, 0, ON, XLARGE, "IN-BERLIN", 9);
        break;
      case 1:
        Serial.println("eLab");
        i = fdu.printString(5, 0, ON, SMALL, "Di 19-22", 8);
        i = fdu.printString(5, 8, ON, SMALL, "Fr 19-01", 8);
        i = fdu.printString(60, 0, ON, XLARGE, "ELAB", 4);
        break;
      case 2:
        Serial.println("BeLUG");
        i = fdu.printString(5, 5, ON, SMALL, "Mi 18-21", 8);
        i = fdu.printString(58, 0, ON, XLARGE, "BELUG", 5);
        break;
      case 3:
        Serial.println("Retro");
        i = fdu.printString(5, 0, ON, SMALL, "last Sa", 7);
        i = fdu.printString(5, 8, ON, SMALL, "15-24", 5);
        i = fdu.printString(58, 0, ON, XLARGE, "RETRO", 5);
        break;
      case 4:
        Serial.println("berlinCreators");
        fdu.writeBitmap(4, 0, 104, 16, sizeof(bitmap_lhvre), &bitmap_lhvre[0]);
        break;
      case 5:
        Serial.println("Coding");
        i = fdu.printString(5, 4, ON, SMALL, "Di 10-20", 8);
        i = fdu.printString(60, 0, ON, XLARGE, "CODE", 4);
        break;
      default:
        fdState = 0;
        break;

    }
    fdu.updatePanel();
  }

}
