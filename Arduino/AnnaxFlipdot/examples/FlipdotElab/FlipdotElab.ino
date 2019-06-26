
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
//   TimeLib:    https://github.com/PaulStoffregen/Time
//   Timezone:   https://github.com/JChristensen/Timezone
//   RTClib.h:   https://github.com/PaulStoffregen/DS1307RTC
//   MemoryFree: https://github.com/McNeight/MemoryFree
//
////////////////////////////////////////////////////////////////////////////
#include <Wire.h>
#include <TimeLib.h>
#include <Timezone.h>    //https://github.com/JChristensen/Timezone
#include <DS1307RTC.h>
#include <MemoryFree.h>  // check memory usage 
#include <SoftwareSerial.h>

#include "Flipdot.h"
#include "FlipdotUtils.h"
#include "SparkFunSerLcd.h"

const byte LED_RED = 3;
const byte LED_GREEN = 6;
const byte LED_BLUE = 7;  // no PWM pin

const byte STOP_PIN = 2;

int i, j;
int inByte;
String commandLine;
//unsigned long previousMillis = 0;        // will store last time from update
int fdState = 0;
int fdMode = 0;
int r, g, b;

FlipDot flipdot(FD_COLUMS, FD_ROWS);
FlipDotUtils fdu(flipdot);

SoftwareSerial mySerial(12, 8); // RX, TX
SparkFunSerLCD lcd;       //instantiate an LCD object

//Central Europe Time Zone (Berlin, Paris)
//TimeChangeRule myDST = {"CEST", Last, Sun, Mar, 2, +120};    //Daylight time = UTC + 2 hours
//TimeChangeRule mySTD = {"CET", Last, Sun, Nov, 2, +60};     //Standard time = UTC + 1 hours
//Timezone myTZ(myDST, mySTD);

//If TimeChangeRules are already stored in EEPROM, comment out the three
//lines above and uncomment the line below.
//Timezone myTZ(100);       //assumes rules stored at EEPROM address 100

//TimeChangeRule *tcr;        //pointer to the time change rule, use to get TZ abbrev
//time_t utc, local;        // utc and local time

DS1307RTC rtc;
//DateTime tm;
unsigned long current_time;
unsigned long last_time;
//int hours, minutes, seconds;
//int days, months, years;
int startTimeOut, stopTimeOut;
boolean timeOut = false;
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

//char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup() {

  mySerial.begin(9600);      //set serial port speed
  lcd.serial = &mySerial;      //set pointer to serial port
  delay(1000);
  lcd.brightness(145);      //set backlight brightness
  lcd.clear();        //clear the screen
  lcd.position(0);      //line 0 character 0
  lcd.serial->print("FlipdotControl");   //first line text
  delay(3000);
  lcd.clear();        //clear the screen

  Serial.begin(115200);
  Serial.println("FlipdotControl v1.1");

  pinMode(STOP_PIN, INPUT_PULLUP);

  flipdot.begin();
  fdu.updatePanel();
  delay(1000);
  setLedColor(0xFF, 0, 0);
  delay(1000);
  setLedColor(0, 0, 0xFF);
  delay(1000);

  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if (timeStatus() != timeSet)
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");

  r = 0;
  g = 0xFF;
  b = 0;
  setLedColor(r, g, b);

  fdu.clearFrameBuffer(OFF);
  i = fdu.printString(5, 0, ON, XLARGE, "IN-BERLIN");
  fdu.updatePanel();
}

void lcdShowState(String str)
{
  lcd.position(4);
  lcd.serial->print(str);
  //while (str.length() < 12)
  //  lcd.serial->write(' ');
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
    case 'C':  fdu.clearFrameBuffer(color); Serial.println("C"); fdu.updatePanel(); break;
    case 'L':  setLedColor(r, g, b); Serial.println("Led"); break;
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

  if (timeStatus() == timeSet) {
    btn = checkButton(STOP_PIN);
    if (btn == true) {
      startTimeOut = hour();
      if (startTimeOut < 22)
        stopTimeOut = hour() + 2;
      else
        stopTimeOut = 1;
      timeOut = true;
      Serial.println("Stop Button pressed!");
    }

    if (timeOut == false) {
      if ((hour() < 18) || (hour() > 22)) {
        printNews();
      }
      else
        lcdShowState("offline  ");
    }
    else {
      lcdShowState("timeout  ");
      if (hour() > stopTimeOut)
        timeOut = false;
    }

    if (hour() < 3 || hour() > 16) {
      setLedColor(r, g, b);
    }
    else {
      setLedColor(0, 0, 0);
    }
  } else {
    Serial.println("The time has not been set.  Please run the Time");
    Serial.println("TimeRTCSet example, or DS1307RTC SetTime example.");
    Serial.println();
  }
  //getTime();
  //showTime();
  //showFreeMem();

  if (true == checkForCommand())
    execCommand();

  //getTimeStr();

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
    default:
      break;
  }

}

boolean checkButton(int buttonPin)
{
  boolean ret = false;
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        ret = true;
      }
    }
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
  return ret;
}


#if 0
void showHelp(void)
{
  Serial.println("Flipdot Control v1.1");
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
  Serial.println("  The command lines is terminated by the \\ character");
  Serial.println("  It gets evaluated after reception of that character");
}
#endif

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
        Serial.println("In-Berlin");
        lcdShowState("In-Berlin");
        i = fdu.printString(5, 0, ON, XLARGE, "IN-BERLIN");
        fdState++;
        break;
      case 1:
        Serial.println("eLab");
        lcdShowState("eLab     ");
        i = fdu.printString(5, 0, ON, XSMALL, "Di 19-22");
        i = fdu.printString(5, 8, ON, XSMALL, "Fr 19-01");
        i = fdu.printString(60, 0, ON, XLARGE, "ELAB");
        fdState++;
        break;
      case 2:
        Serial.println("BeLUG");
        lcdShowState("BeLUG    ");
        i = fdu.printString(5, 5, ON, XSMALL, "Mi 18-21");
        i = fdu.printString(58, 0, ON, XLARGE, "BELUG");
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

void showFreeMem(void) {
  static unsigned long previousMillis = 0;        // will store last time from update
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 30000) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    Serial.print("freeMemory()=");
    Serial.println(freeMemory());
    lcd.position(0);      //line 0 character 0
    lcd.serial->print(freeMemory());
  }
}

void setLedColor(uint16_t red, uint16_t green, uint16_t blue)
{
  analogWrite(LED_RED, red);
  analogWrite(LED_GREEN, green);
  analogWrite(LED_BLUE, blue);
}
