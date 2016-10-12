#ifndef __FLIPDOT_H__
#define __FLIPDOT_H__

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif 

const byte LATCH = 10;
const byte OE = 9;

//#define FD_COLUMS 50
//#define FD_PANELS 2 
#define FD_COLUMS 115
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
    FlipDot(int _latchPin, int _oePin);
    void Update(void);
    void UpdatePanel(int panel);
    
    byte fdMtx[2][FD_COLUMS];

  private:
    void ShiftOut(void);
    
//    byte fdPanelSize[FD_PANELS] = {25, 25}; //, 20, 20, 25};
//    byte fdPanelSelect[FD_PANELS] = {FD_PANEL1_SIG, FD_PANEL2_SIG}; //, FD_PANEL3_SIG, FD_PANEL4_SIG, FD_PANEL5_SIG};
    byte fdPanelSize[FD_PANELS] = {25, 25, 20, 20, 25};
    byte fdPanelSelect[FD_PANELS] = {FD_PANEL1_SIG, FD_PANEL2_SIG, FD_PANEL3_SIG, FD_PANEL4_SIG, FD_PANEL5_SIG};

    byte fdRow1, fdRow2, fdCtrl;
 
    int latchPin, oePin;
};

#endif /* __FLIPDOT_H__ */

