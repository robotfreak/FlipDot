#ifndef __FLIPDOT_H__
#define __FLIPDOT_H__

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif 

const byte LATCH = 10;
const byte OE = 9;
const byte RES = 6;
const byte COL = 4;


#define FLIPDOT_BLACK          0
#define FLIPDOT_YELLOW         1  
//#define FD_COLUMS 50
//#define FD_PANELS 2 
#define FD_COLUMS 115
#define FD_ROWS 16
#define FD_PANELS 5 
#define FD_PANEL_MASK 0xF8
#define FD_PANEL5_SIG 7
#define FD_PANEL4_SIG 6
#define FD_PANEL3_SIG 5
#define FD_PANEL2_SIG 4
#define FD_PANEL1_SIG 3
#define FD_COLUMN_SIG 2
#define FD_FRAME_SIG 1
#define FD_STROBE_SIG 0

class FlipDot {
  public: 
    FlipDot(int _sizeX, int _sizeY);
    void begin();
    void drawPixel(int16_t, int16_t, uint16_t); 
    void setPixel(int16_t x, int16_t y, uint16_t color);

    void update(void);
    void updatePanel(int panel);
    
  private:
    byte bitReverse(byte x);
    void ShiftOut(void);
    
//    byte fdPanelSize[FD_PANELS] = {25, 25}; //, 20, 20, 25};
//    byte fdPanelSelect[FD_PANELS] = {FD_PANEL1_SIG, FD_PANEL2_SIG}; //, FD_PANEL3_SIG, FD_PANEL4_SIG, FD_PANEL5_SIG};
    byte fdPanelSize[FD_PANELS] = {25, 25, 20, 20, 25};
    byte fdPanelSelect[FD_PANELS] = {FD_PANEL1_SIG, FD_PANEL2_SIG, FD_PANEL3_SIG, FD_PANEL4_SIG, FD_PANEL5_SIG};
    boolean *displayBuffer = NULL;
 
    byte fdMtx[2][FD_COLUMS];

    byte fdRow1, fdRow2, fdCtrl;
 
    int latchPin, oePin, resPin, colPin;
    int sizeX, sizeY;
};

#endif /* __FLIPDOT_H__ */

