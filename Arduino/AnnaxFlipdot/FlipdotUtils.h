#ifndef __FLIPDOTUTILS_H__
#define __FLIPDOTUTILS_H__

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif 

//#include "Flipdot.h"

//================== Constants ===============================
#define X_SIZE 115    // 115 column
#define Y_SIZE 2      // 16 rows (represented by 2 bytes)
#define Y_PIXELS 16   // True Y-Size if the display
#define OFF 0
#define ON 1

class FlipDot;

class FlipDotUtils {
  public: 
    FlipDotUtils(FlipDot & _flipdot);
    void addFlipdot(FlipDot & _flipdot);
    void setSerialDebug(bool flag);
    void clearFrameBuffer(int color);
    void updatePanel(void);
    void setPixel(int x, int y, int color);
    int hex2int(char *hex);
    int writeBitmap(int xOffs, int yOffs, int xSize, int ySize, int size, char *buf);
    int printBitmap(int xOffs, int yOffs, int color, int xSize, int ySize, String s);
    void hLine(int y, int color);
    void vLine(int x, int color);
    void shiftFrameBuffer(int fs);
    void scrollFrameBuffer();
    int printString(int xOffs, int yOffs, int color, int size, char * s, int slen);
    void printFrameBuffer(void);
    void setFrameBuffer(int x, int y, int value);
    int getFrameBuffer(int x, int y);

    // frameBuffer stores the content of the Flipdotmatrix
    // Each pixel is one bit (I.e only an eigth of storage is required
    unsigned char frameBuffer[X_SIZE][Y_SIZE];

    
  private:
    int printChar3x5(int xOffs, int yOffs, int color, unsigned char c);
    int printChar5x7(int xOffs, int yOffs, int color, unsigned char c);
    int printChar6x8v(int xOffs, int yOffs, int color, unsigned char c);
    //int printChar8x8(int xOffs, int yOffs, int color, unsigned char c);
    int printChar8x12(int xOffs, int yOffs, int color, unsigned char c);
    int printChar9x16(int xOffs, int yOffs, int color, unsigned char c);

    FlipDot * flipdot;
    bool serialDebug = false;

};

#endif /* __FLIPDOTUTILS_H__ */

