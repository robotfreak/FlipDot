
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
//   RTClib.h:   https://github.com/PaulStoffregen/DS1307RTC
//   MemoryFree: https://github.com/McNeight/MemoryFree
//
////////////////////////////////////////////////////////////////////////////
#include <Wire.h>
#include <TimeLib.h>
#include <RTClib.h>
#include <MemoryFree.h>  // check memory usage 

#include "Flipdot.h"

int i, j;
int inByte;
String commandLine;
//unsigned long previousMillis = 0;        // will store last time from update
int fdState = 0;
int fdMode = 0;
int r, g, b;


FlipDot flipdot(FD_COLUMS, FD_ROWS);

RTC_DS1307 rtc;
DateTime tm;
unsigned long current_time;
unsigned long last_time;
int hours, minutes, seconds;
int days, months, years;

//char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup() {

  Serial.begin(115200);
  Serial.println("FlipdotControl v1.0");
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());

  flipdot.begin();
  flipdot.update();
  delay(1000);
  flipdot.setLedColor(0xFF, 0, 0);
  delay(1000);
  flipdot.setLedColor(0, 0, 0xFF);
  delay(1000);

  if (! rtc.begin()) {
  }
  else if (! rtc.isrunning()) {
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }


  r = 0;
  g = 0xFF;
  b = 0;
  flipdot.setLedColor(r, g, b);
  //
  //  while(1)
  //    printHello();

  clearFrameBuffer(OFF);
  i = printString(5, 0, ON, XLARGE, "IN-BERLIN");
  flipdot.update();
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());

}

void pollCommand() {
  char c;
  int color;
  unsigned char cmd;
  int cmdPtr;
  int xVal, yVal;
  int xSiz, ySiz;
  char fontSize;
  int fsize;

  String xStr, yStr, str;
  String xSizStr, ySizStr;
  String outputString;

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
        case 'L':  flipdot.setLedColor(r, g, b); Serial.println("Led"); break;
        case 'S':  setPixel(xVal, yVal, color); break;
        case 'H':  hLine(yVal, color); updatePanel(); Serial.println("H"); break;
        case 'V':  vLine(xVal, color); updatePanel(); Serial.println("V"); break;
        case 'P':  printString(xVal, yVal, color, fsize, outputString); updatePanel(); Serial.println("P");  break;
        case 'B':  printBitmap(xVal, yVal, color, xSiz, ySiz, outputString); updatePanel(); Serial.println("B"); break;
        case 'U':  updatePanel(); Serial.println("U"); break;
        case 'f':  fdMode = 1; break;
        case 'n':  fdMode = 2; break;
        case 't':  fdMode = 3; break;
        case 'd':  fdMode = 4; break;
          // case 'h':  showHelp(); break;
      }
    }
  }
}

void loop() {

  showFreeMem();
  showTime();
  pollCommand();

  tm = rtc.now();
  hours = tm.hour();
  minutes = tm.minute();
  seconds = tm.second();
  //getTimeStr();

  if ((hours < 18) || (hours > 20))
  {
    printNews();
  }

  if (hours < 3 || hours > 16)
  {
    flipdot.setLedColor(r, g, b);

  }
  else
  {
    flipdot.setLedColor(0, 0, 0);

  }


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
      printDateTime();
      break;
    default:
      break;
  }

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

String getTimeStr(void) {
  String str = "";
  tm = rtc.now();
  hours = tm.hour();
  minutes = tm.minute();
  seconds = tm.second();
  print2digits(hours);
  Serial.write(':');
  print2digits(minutes);
  Serial.write(':');
  print2digits(seconds);
  Serial.println("\n");
  if (hours < 10)
    str += "0";
  str += String(hours);
  str += ":";
  if (minutes < 10)
    str += "0";
  str += String(minutes);
  return str;
}

String getDateStr(void) {
  String str = "";
  //  tm = rtc.now();
  //  day = tm.day();
  //  year = tm.year();
  //  month = tm.month();
  //  if (day < 10)
  //    str += "0";
  str += String(day());
  str += ".";
  //  if (month < 10)
  //    str += "0";
  str += String(month());
  str += ".";
  str += String(year());
  return str;
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
//===================================
// For debugging and testing only
//===================================
void printNews() {
  static unsigned long previousMillis = 0;        // will store last time from update

  unsigned long currentMillis = millis();
  String str;

  if (currentMillis - previousMillis >= 30000) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    clearFrameBuffer(OFF);
    switch (fdState)
    {
      case 0:
        Serial.println("In-Berlin");
        i = printString(5, 0, ON, XLARGE, "IN-BERLIN");
        fdState = 1;
        break;
      case 1:
        Serial.println("eLab");
        i = printString(5, 0, ON, XSMALL, "Di 19-22");
        i = printString(5, 8, ON, XSMALL, "Fr 19-01");
        i = printString(60, 0, ON, XLARGE, "ELAB");
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
void printDateTime(void) {
  static unsigned long previousMillis = 0;        // will store last time from update

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 10000) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    switch (fdState)
    {
      case 0:
        getTimeStr();
        fdState = 1;
        break;
      case 1:
        getDateStr();
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

void showFreeMem(void) {
  static unsigned long previousMillis = 0;        // will store last time from update
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 30000) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    Serial.print("freeMemory()=");
    Serial.println(freeMemory());
  }
}

void showTime(void) {
  static unsigned long previousMillis = 0;        // will store last time from update
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 30000) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    tm = rtc.now();
    hours = tm.hour();
    minutes = tm.minute();
    print2digits(hours);
    Serial.write(':');
    print2digits(minutes);
    Serial.println("\n");
  }
}

