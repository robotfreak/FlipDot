
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

int fdState = 0;
int fdMode = 0;

int r, g, b;

SoftwareSerial mySerial(A5, A4); // RX, TX

FlipDot flipdot(FD_COLUMS, FD_ROWS);
FlipDotUtils fdu(flipdot);
/*
const unsigned char epd_bitmap_Bitmap [] PROGMEM = {
	// 'bC-Logo-Pixel-115x16-grSchw-2, 104x16px
	0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x10, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x88, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xc0, 0x00, 0x0c, 0x00, 0x06, 0x30, 0x1f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 
	0x0c, 0xc0, 0xec, 0x1b, 0xbc, 0x78, 0x00, 0x00, 0x20, 0x00, 0x00, 0xc0, 0x00, 0x0c, 0xc0, 0x08, 
	0x08, 0x70, 0x18, 0x00, 0x00, 0x60, 0x00, 0x00, 0xc0, 0x70, 0xec, 0x1f, 0x08, 0x08, 0x70, 0x01, 
	0xce, 0x7c, 0xf9, 0xe1, 0xcf, 0xfc, 0xf9, 0xed, 0xdf, 0x88, 0x08, 0x60, 0x03, 0xdf, 0x7e, 0xfb, 
	0xf3, 0xdf, 0xfe, 0xd9, 0x8d, 0xd9, 0x8c, 0x18, 0x60, 0x03, 0x1b, 0x06, 0x67, 0x3b, 0x18, 0xc6, 
	0xf9, 0x8c, 0xd9, 0x86, 0x30, 0x70, 0x03, 0x1f, 0x3e, 0x66, 0x1b, 0x1e, 0xc6, 0xf1, 0x8c, 0xd9, 
	0x82, 0x20, 0x70, 0x1b, 0x1e, 0x7e, 0x66, 0x1b, 0x0f, 0xc6, 0xc1, 0x8c, 0xd9, 0x83, 0xe0, 0x38, 
	0x7b, 0x18, 0x66, 0x67, 0x3b, 0x03, 0xfe, 0xf9, 0x8e, 0xd9, 0x83, 0xe0, 0x1f, 0xf3, 0x1f, 0x7e, 
	0x7b, 0xf3, 0x1f, 0x7c, 0x79, 0x86, 0xd9, 0x81, 0xc0, 0x0f, 0xc3, 0x0f, 0x3e, 0x39, 0xe3, 0x1e
};

const GFXglyph epd_bitmap_Glyphs [] PROGMEM = {
	{ 0, 104, 16, 0, '0' }// 'bC-Logo-Pixel-115x16-grSchw-2'
};

const GFXfont epd_bitmap_Font PROGMEM = {
	(uint8_t *)epd_bitmap_Bitmap,
	(GFXglyph *)epd_bitmap_Glyphs,
	1
};
*/
/*
// 'bC-Logo-', 104x16px
const unsigned char epd_bitmap_bC_Logo [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x10, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x88, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xc0, 0x00, 0x0c, 0x00, 0x06, 0x30, 0x1f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 
	0x0c, 0xc0, 0xec, 0x1b, 0xbc, 0x78, 0x00, 0x00, 0x20, 0x00, 0x00, 0xc0, 0x00, 0x0c, 0xc0, 0x08, 
	0x08, 0x70, 0x18, 0x00, 0x00, 0x60, 0x00, 0x00, 0xc0, 0x70, 0xec, 0x1f, 0x08, 0x08, 0x70, 0x01, 
	0xce, 0x7c, 0xf9, 0xe1, 0xcf, 0xfc, 0xf9, 0xed, 0xdf, 0x88, 0x08, 0x60, 0x03, 0xdf, 0x7e, 0xfb, 
	0xf3, 0xdf, 0xfe, 0xd9, 0x8d, 0xd9, 0x8c, 0x18, 0x60, 0x03, 0x1b, 0x06, 0x67, 0x3b, 0x18, 0xc6, 
	0xf9, 0x8c, 0xd9, 0x86, 0x30, 0x70, 0x03, 0x1f, 0x3e, 0x66, 0x1b, 0x1e, 0xc6, 0xf1, 0x8c, 0xd9, 
	0x82, 0x20, 0x70, 0x1b, 0x1e, 0x7e, 0x66, 0x1b, 0x0f, 0xc6, 0xc1, 0x8c, 0xd9, 0x83, 0xe0, 0x38, 
	0x7b, 0x18, 0x66, 0x67, 0x3b, 0x03, 0xfe, 0xf9, 0x8e, 0xd9, 0x83, 0xe0, 0x1f, 0xf3, 0x1f, 0x7e, 
	0x7b, 0xf3, 0x1f, 0x7c, 0x79, 0x86, 0xd9, 0x81, 0xc0, 0x0f, 0xc3, 0x0f, 0x3e, 0x39, 0xe3, 0x1e
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 224)
const int epd_bitmap_allArray_LEN = 1;
const unsigned char* epd_bitmap_allArray[1] = {
	epd_bitmap_bC_Logo
};
*/
/**
 * Made with Marlin Bitmap Converter
 * https://marlinfw.org/tools/u8glib/converter.html
 *
 * This bitmap from the file 'bC-Logo-Pixel-115x16-grSchw-2.png'
 */
#pragma once

#define LHVRE_BMPWIDTH  104

const unsigned char bitmap_lhvre[] PROGMEM = {
  B00000000,B00000000,B00000000,B00000000,B00000000,B10000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,B00010000,B10000100,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,B00001000,B10001000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,B00000011,B11100000,B00001111,B11100000,B00000000,B00000000,B00000000,B00000000,B00000000,
  B11000000,B00000000,B00001100,B00000000,B00000110,B00110000,B00011111,B11110000,B00000000,B00000000,B00000000,B00000000,B00000000,
  B11000000,B00000000,B00001100,B11000000,B11101100,B00011011,B10111100,B01111000,B00000000,B00000000,B00100000,B00000000,B00000000,
  B11000000,B00000000,B00001100,B11000000,B00001000,B00001000,B01110000,B00011000,B00000000,B00000000,B01100000,B00000000,B00000000,
  B11000000,B01110000,B11101100,B00011111,B00001000,B00001000,B01110000,B00000001,B11001110,B01111100,B11111001,B11100001,B11001111,
  B11111100,B11111001,B11101101,B11011111,B10001000,B00001000,B01100000,B00000011,B11011111,B01111110,B11111011,B11110011,B11011111,
  B11111110,B11011001,B10001101,B11011001,B10001100,B00011000,B01100000,B00000011,B00011011,B00000110,B01100111,B00111011,B00011000,
  B11000110,B11111001,B10001100,B11011001,B10000110,B00110000,B01110000,B00000011,B00011111,B00111110,B01100110,B00011011,B00011110,
  B11000110,B11110001,B10001100,B11011001,B10000010,B00100000,B01110000,B00011011,B00011110,B01111110,B01100110,B00011011,B00001111,
  B11000110,B11000001,B10001100,B11011001,B10000011,B11100000,B00111000,B01111011,B00011000,B01100110,B01100111,B00111011,B00000011,
  B11111110,B11111001,B10001110,B11011001,B10000011,B11100000,B00011111,B11110011,B00011111,B01111110,B01111011,B11110011,B00011111,
  B01111100,B01111001,B10000110,B11011001,B10000001,B11000000,B00001111,B11000011,B00001111,B00111110,B00111001,B11100011,B00011110
};

void setup() {

  Serial.begin(115200);
  Serial.println("FlipdotTwitter v0.3");
  mySerial.begin(9600);

  r = 0;
  g = 0xFF;
  b = 0;
  setLedColor(r, g, b);

  flipdot.begin();
  fdu.setSerialDebug(true);
  fdu.updatePanel();
  fdu.clearFrameBuffer(OFF);
  printNews(4);
  //i = fdu.printString(5, 0, ON, XLARGE, "ELAB");
  //fdu.updatePanel();
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
    if (commandLine.length() < 100) {
      if (c != '\r' || c != '\n')
        commandLine += c;
    }
    else {
      commandLine = "";
      Serial.print("Cmd too long");
    }

    // ==== If command string is complete... =======
    if (c == '\n' || c == '\r') {
      if (commandLine.charAt(0) != '>') {
        commandLine += '\n';
        //Serial.println("Cmd complete"); 
        ret = true;
      }
      else {
        commandLine = "";
      }
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

  //Serial.print("CMD: ");
  //Serial.println(commandLine);

  cmd = commandLine.charAt(0);
  if (cmd == '>') {
    commandLine = "";    // Reset command mode
    return;
  }
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
  else if (cmd == 'P')
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

  fdMode = 0;
  fdState = 0;
  // ======= Execute the respective command ========
  switch (cmd) {
    case 'C':  fdu.clearFrameBuffer(color); Serial.println("C"); fdu.updatePanel(); break;
    case 'S':  fdu.setPixel(xVal, yVal, color); fdu.updatePanel(); break;
    case 'H':  fdu.hLine(yVal, color); fdu.updatePanel(); Serial.println("H"); break;
    case 'V':  fdu.vLine(xVal, color); fdu.updatePanel(); Serial.println("V"); break;
    case 'P':  if (outputString.length() > 19) scrollText(outputString); else fdu.printString(xVal, yVal, color, fsize, outputString); fdu.updatePanel(); Serial.println("P");  break;
    case 'B':  fdu.printBitmap(xVal, yVal, color, xSiz, ySiz, outputString); fdu.updatePanel(); Serial.println("B"); break;
    case 'L':  if (color == 'r') setLedColor(0xff,0,0); 
               else if (color == 'g') setLedColor(0, 0xff,0); 
               else if (color == 'b') setLedColor(0, 0, 0xff);
               else setLedColor(0, 0,0); 
               break;
    case 'U':  fdu.updatePanel(); Serial.println("U"); break;
    case 'f':  fdMode = 1; break;
    case 'n':  printNews(xVal); break;
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
    fdu.printString(0, 0, ON, XSMALL, &textBuf[0] );
    ofs += 19;
    textBufLen -= 19;
    fdu.printString(0, 8, ON, XSMALL, &textBuf[ofs] );
    ofs += 19;
    textBufLen -= 19;
    fdu.updatePanel();
    delay(10000);
    while (textBufLen > 0)
    {
      fdu.scrollFrameBuffer();
      fdu.printString(0, 8, ON, XSMALL, &textBuf[ofs] );
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
        i = fdu.printString(5, 0, ON, XLARGE, "IN-BERLIN");
        fdState++;
        break;
      case 1:
        Serial.println("eLab");
        i = fdu.printString(5, 0, ON, SMALL, "Di 19-22");
        i = fdu.printString(5, 8, ON, SMALL, "Fr 19-01");
        i = fdu.printString(60, 0, ON, XLARGE, "ELAB");
        fdState++;
        break;
      case 2:
        Serial.println("BeLUG");
        i = fdu.printString(5, 5, ON, SMALL, "Mi 18-21");
        i = fdu.printString(58, 0, ON, XLARGE, "BELUG");
        fdState++;
        break;
      case 3:
        Serial.println("Retro");
        i = fdu.printString(5, 0, ON, SMALL, "last Sa");
        i = fdu.printString(5, 8, ON, SMALL, "15-24");
        i = fdu.printString(58, 0, ON, XLARGE, "RETRO");
        fdState = 0;
        break;
      case 4:
        Serial.println("berlinCreators");
        fdu.writeBitmap(4,0,104,16,sizeof(bitmap_lhvre),&bitmap_lhvre[0]);

/*        
        i = fdu.printBitmap( 4,0,ON,8,8,"0000000000C0C0C0");
        i = fdu.printBitmap( 4,8,ON,8,8,"C0FCFEC6C6C6FE7C");
        i = fdu.printBitmap(12,0,ON,8,8,"0000000000000000");
        i = fdu.printBitmap(12,8,ON,8,8,"70F9D9F9F1C1F979");
        i = fdu.printBitmap(20,0,ON,8,8,"00000000000C0C0C");
        i = fdu.printBitmap(20,8,ON,8,8,"ECED8D8C8C8C8E86");
        i = fdu.printBitmap(28,0,ON,8,8,"000000000000C0C0");
        i = fdu.printBitmap(28,8,ON,8,8,"1FDFD9D9D9D9D9D9");
        i = fdu.printBitmap(36,0,ON,8,8,"001008000306EC08");
        i = fdu.printBitmap(36,8,ON,8,8,"08888C8682838381");
        i = fdu.printBitmap(44,0,ON,8,8,"80848800E0301B08");
        i = fdu.printBitmap(44,8,ON,8,8,"0808183020E0E0C0");
        i = fdu.printBitmap(52,0,ON,8,8,"000000000F1FBC70");
        i = fdu.printBitmap(52,8,ON,8,8,"7060607070381F0F");
        i = fdu.printBitmap(60,0,ON,8,8,"00000000E0F07818");
        i = fdu.printBitmap(60,8,ON,8,8,"010303031B7BF3C3");
        i = fdu.printBitmap(68,0,ON,8,8,"0000000000000000");
        i = fdu.printBitmap(68,8,ON,8,8,"CEDF1B1F1E181F0F");
        i = fdu.printBitmap(76,0,ON,8,8,"0000000000000000");
        i = fdu.printBitmap(76,8,ON,8,8,"7C7E063E7E667E3E");
        i = fdu.printBitmap(84,0,ON,8,8,"0000000000002060");
        i = fdu.printBitmap(84,8,ON,8,8,"F9FB676666677B39");
        i = fdu.printBitmap(92,0,ON,8,8,"0000000000000000");
        i = fdu.printBitmap(92,8,ON,8,8,"E1F33B1B1B3BF3E3");
        i = fdu.printBitmap(100,0,ON,8,8,"0000000000000000");
        i = fdu.printBitmap(100,8,ON,8,8,"CFDF181E0F031F1E");
        i = fdu.printBitmap(108,0,ON,8,8,"0000000000000000");
        i = fdu.printBitmap(108,8,ON,8,8,"0000000000000000");
*/        
        break;
      default:
        fdState = 0;
        break;

    }
    fdu.updatePanel();
  }

}

#if 0
//===================================
// For debugging and testing only
//===================================
void printTest(state) {
  static unsigned long previousMillis = 0;        // will store last time from update
  unsigned long currentMillis = millis();
  bool trigger = false;

  if (state >= 0) {
    fdState = state;
    trigger = true;
  }
  else if (currentMillis - previousMillis >= 10000) {
    previousMillis = currentMillis;
    trigger = true;
  }
  if (trigger) {
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
#endif
