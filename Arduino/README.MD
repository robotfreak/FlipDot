# Programm Übersicht

* FlipDotTest: Togglen aller Flipdots (an/aus)
* FlipDotGFXDemo: Adafruit GFX library Demo
* FlipDotGameOfLifeGFX: Game of life Simulation mit Adafruit GFX library 
* FlipDotPongGFX: Pong Game mit Adafruit GFX library 
* FlipDotTetrisGFX: Tetris mit with Adafruit GFX library 
* FlipdotClockRTCTemp: Anzeige von Uhrzeit, Temperatur und Öuftfeuchtigkeit
* FlipdotDemo: Fernbedienung über serielle Schnittstelle, Ausgabe von Text, Grafik mit Arduino Nano
* FlipdotCtrlNodeMCU: Fernbedienung über serielle Schnittstelle oder WiFi (Telnet), Ausgabe von Text, Grafik mit NodeMCU (ESP8266)

# FlipDot API

## Konstruktor: 

 FlipDot flipdot(FD_COLS, FD_ROWS);  // Number of Rows (up to 115), Number of Colums (16) 
 
## Funktionen:
* flipdot.begin();                  // Start Flipdot, initialize ports
* flipdot.setPanels(int num);       // use only <num> numbers of panels. Speed up the Display 
* flipdot.update();                 // updates all flipdot panels
* flipdot.setSerialDebug(bool val); // <true> enabled serial debug output, <false> disabled serial debug output (speeds up display)
* flipdot.setPixel(x,y,color);      // set/clear a pixel YELLOW to switch pixel on, BLACK to switch pixel off)
* flipdot.drawPixel(x,y,color);     // set/clear a pixel YELLOW to switch pixel on, BLACK to switch pixel off) for Adafruit GFX library
* ...
 
All functions of AdafruitGFX library can be used, too:
* flipdot.setCursor(int x, int y); 
* flipdot.setTextSize(int);
* flipdot.println(char *);
* ...


# Bedien Nachrichten FlipdotDemo

FlipdotDemo basiert auf dem FlipdotHack von C-Hack http://c-hack.de/projekte/flipdot-hack/ 

```
 Command format
   <Command>,<Color>,<X>,<Y>,[<size>,<xsiz>,<ysiz>,]<....string....>\n

   Commands:
     C  Clear Screen
     B  Draw a Bitmap
     P  Print Text
     H  Draw a horizontal line
     V  Draw a vertical line
     S  Set a pixel
     U  Update Panel
   Color:
     B  Black
     Y  Yellow
   X,Y:
     Required for all Print or Bitmap commands
     X (0..114) column
     Y (0..15) row
     Only Y is required for the horizontal line command "H"
     Only X is required for the vertical line command "V"
   size:
     Required only for Print commands
     S SMALL
     M MEDIUM
     L LARGE
     X EXTRALARGE
   xsiz,ysiz:
     Required only for Bitmap commands
     xsiz number of bitmap columns
     ysiz number of bitmap rows
     
   String:
     Contains the characters (for text) or pixels (for graphics) to be printed.
     Bitmap Strings are interpreted as Hexadecimal values.
     For Bitmaps <= 8 pixel wide use 1 Byte (2 ASCII chars) for each single row  
     For Bitmaps > 8 .. <= 16 pixel wide use 2 Byte (4 ASCII chars) for each single row  
     For Bitmaps > 16 .. <= 32 pixel wide use 4 Byte (8 ASCII chars) for each single row ...
   "\n":
     The command lines is terminated by the return character
     It gets evaluated after reception of that character
  
    Examples:
    "C\n" all pixels black, clears the screen
    "C,Y\n" all pixels yellow
    "P,Y,0,1,M,Hallo Welt\n" Prints String "Hallo Welt" at position x=0 (column), y=1 (row)
    "B,Y,10,1,8,8,AA55AA55AA55AA55\n" prints a checkerboard bitmap at position x=10 (column), y=1 (row), size 8x8 pixel
    "H,Y,0,0\n" prints a horizontal line at row 0
    "Y,Y,50,0\n" prints a vertical line at column 50
    "T\n" starts a test programm showing some text output of different font size and simple graphics
  ```  
