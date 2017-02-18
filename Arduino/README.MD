# Programm Übersicht

* FlipdotClockRTCTemp Anzeige von Uhrzeit, Temperatur und Öuftfeuchtigkeit
* FlipdotTest Togglen aller Flipdots (an/aus)
* FlipdotCtrl Fernbedienung über serielle Schnittstelle, Ausgabe von Text, Grafik mit Arduino Nano
* FlipdotCtrlNodeMCU Fernbedienung über serielle Schnittstelle, Ausgabe von Text, Grafik mit NodeMCU (ESP8266)

# Bedien Nachrichten FlipdotCtrl

```
 Command format
   <Command>,<Color>,<x>,<y>,<size>,<....string....>\n

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
     Required for all Print commands
     Only Y is required for the horizontal line command "H"
     Only X is required for the vertical line command "V"
   size:
     S SMALL
     M MEDIUM
     L LARGE
     X EXTRALARGE
   String:
     Contains the characters to be printed
   "\":
     The command lines is terminated by the return character
     It gets evaluated after reception of that character
  
    Examples:
    "C\" all pixels black, clears the screen
    "C,Y\" all pixels yellow
    "P,Y,0,1,M,Hallo Welt\" Prints String "Hallo Welt" at position x=0 (column), y=1 (row)
    "B,Y,10,1,8,8,AA55AA55AA55AA55\" prints a checkerboard bitmap at position x=10 (column), y=1 (row), size 8x8 pixel
    "H,Y,0,0\" prints a horizontal line at row 0
    "Y,Y,50,0\" prints a vertical line at clumn 50
  ```  
