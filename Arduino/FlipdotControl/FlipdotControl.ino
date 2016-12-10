////////////////////////////////////////////////////////////////////////////
// Flipdot_Firmware
// CC-BY-SA-NC    8/2016 C-Hack (Ralf@Surasto.de)
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

int i,j;
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
  char fontSize;
  int fsize;

  String xStr,yStr;
  String outputString;
  
  if (Serial.available() > 0) {
    c = Serial.read();
    if (commandLine.length()<100) {
      commandLine += c;
    }
    else {
      commandLine = "";
      Serial.print("?");
    }

    // ==== If command string is complete... =======
    if (c=='\\') {

      cmd = commandLine.charAt(0);
      if (commandLine.charAt(2) == 'B') color = 1; else color = 0;
      cmdPtr=4;
      xStr = ""; yStr = "";
      while ((cmdPtr<commandLine.length()) && (commandLine.charAt(cmdPtr)!=',')) {
        xStr +=  (char)commandLine.charAt(cmdPtr);
        cmdPtr++;
        xVal = xStr.toInt();
      }
      
      cmdPtr++;
      while ((cmdPtr<commandLine.length()) && (commandLine.charAt(cmdPtr)!=',')) {
        yStr += (char)commandLine.charAt(cmdPtr);
        cmdPtr++;
        yVal = yStr.toInt();
      }
      
      cmdPtr++;
      fontSize = commandLine.charAt(cmdPtr);
      if (fontSize == 'S') fsize = SMALL;
      else if (fontSize == 'M') fsize = MEDIUM;
      else if (fontSize == 'L') fsize = LARGE;
      else fsize = EXTRALARGE;
      
      cmdPtr+=2;
      outputString="";
      while ((cmdPtr<commandLine.length()-1)&&(outputString.length()<100)) {
        outputString += (char)commandLine.charAt(cmdPtr);
        cmdPtr++;
      }
    
      commandLine = "";    // Reset command mode

      // ======= Debug only ===========
      Serial.println((char)cmd);
      Serial.println(color);
      Serial.println(xVal);
      Serial.println(yVal);
      Serial.println(fontSize);
      Serial.println(outputString);
    
      // ======= Execute the respective command ========
      switch (cmd) {
        case 'C':  clearAll(color); Serial.println("C"); updatePanel(); break;
        case 'T':  printTest(yVal); Serial.println("T"); updatePanel(); break;
        case 'S':  setPixel(xVal,yVal,color); break;
        case 'H':  hLine(yVal,color); updatePanel(); break;
        case 'V':  vLine(xVal,color); updatePanel(); break;
        case 'P':  printString(xVal, yVal, color, fsize, outputString); updatePanel(); break;
        case 'U':  updatePanel(); Serial.println("U"); break;
      }
    }
  }
}


//===================================
// For debugging and testing only
//===================================
void printTest(int y) {   
  int i,j;
      
    clearAll(OFF);
//    printFont();
    hLine(y,1);
    i = printString(2,1,BLACK,MEDIUM,"Here is a short Text String !");
//    i = printString(2,15,YELLOW,LARGE,"Noch ein Test \x81");
 //   i = printString(2,18,ON,"Passt das noch ?");
//    i=printChar(10,2,ON,'A');
    printFrameBuffer();
}