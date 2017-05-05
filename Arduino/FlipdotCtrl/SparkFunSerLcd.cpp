#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "SparkFunSerLcd.h"

SparkFunSerLCD::SparkFunSerLCD(){
}

void SparkFunSerLCD::brightness(uint8_t b){
  if(b>=128 && b<=157){
    serial->write(0x7C);
    serial->write(b);
    delay(10);
  }
}

void SparkFunSerLCD::splash(){
  serial->write(0x7C);
  serial->write(0x0A);
  delay(10);
}

void SparkFunSerLCD::clear(){
  serial->write(0xFE);
  serial->write(0x01);
}

void SparkFunSerLCD::position(uint8_t p){
  serial->write(0xFE);
  if(p<16)
    serial->write(p+128);
  else if(p<32)
    serial->write(p+48+128);
  else
    serial->write(128);
}

