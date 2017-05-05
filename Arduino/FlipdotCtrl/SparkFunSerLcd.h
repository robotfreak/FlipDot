#include <SoftwareSerial.h>

 class SparkFunSerLCD{     //basic lcd class
public:
  SparkFunSerLCD();   //default constructor
  SoftwareSerial *serial;   //pointer to serial port
  void brightness(uint8_t); //set backlight brightness
  void splash();      //update splash screen
  void clear();     //clear the screen
  void position(uint8_t);   //set the cursor position
};

