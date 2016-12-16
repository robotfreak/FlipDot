/////////////////////////////////////////////////////////////////
//  Testenvironment and base for displaying Fonts on the
//  Flipdot display. Here the content of the display is printed
//  to stdout.
//  CC-BY SA NC 2016 c-hack.de    ralf@surasto.de
//  added support for 9x16 font   by RobotFreak
//  added support for graphics    by RobotFreak
/////////////////////////////////////////////////////////////////
#ifdef _WIN32
#include <windows.h> 
#endif

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include "font5x8v.h"
#include "font6x8.h"
#include "font6x8v.h"
#include "font8x8.h"
#include "font8x12.h"
#include "font9x16.h"

//================== Constants ===============================
#define X_SIZE 78   // 128 column
#define Y_SIZE 2    // 28 rows (represented by 4 bytes)
#define Y_PIXELS 16 // True Y-Size if the display
#define OFF 0
#define ON 1
#define XSMALL 0
#define SMALL 1
#define MEDIUM 2
#define LARGE 3
#define XLARGE 4

//================ global Variables ==========================
// frameBuffer stores the content of the Flipdotmatrix
// Each pixel is one bit (I.e only an eigth of storage is required
unsigned char frameBuffer[X_SIZE][Y_SIZE];

//===========================================
// clearFrameBuffer(int color)
// Sets all bits to
//     Yellow if color = ON
//     Black if color  = OFF
//===========================================
void clearFrameBuffer(int color)
{
    int x, y;
    for (x = 0; x < X_SIZE; x++)
        for (y = 0; y < Y_SIZE; y++)
        {
            if (color == ON)
                frameBuffer[x][y] = 0xFF;
            else
                frameBuffer[x][y] = 0x00;
        }
}

//===========================================
// setFrameBuffer(int x, int y, int value)
// Set one Pixel at x,y-Position
// value can be ON or OFF
//===========================================
void setFrameBuffer(int x, int y, int value)
{
    unsigned char w, wNot;
    int yByteNo, yBitNo;

    w = 1;
    if ((y < 8 * Y_SIZE) && (x < X_SIZE) && (x >= 0) && (y >= 0))
    {
        yByteNo = y / 8; // integer division to select the byte
        yBitNo = y % 8;  // module division (residual) to select the bit in that byte
        w = w << yBitNo;
        if (value == ON)
        {
            frameBuffer[x][yByteNo] = frameBuffer[x][yByteNo] | w; // Logical OR adds one bit to the existing byte
        }
        else
        {
            wNot = 0xFF - w;
            frameBuffer[x][yByteNo] = frameBuffer[x][yByteNo] & wNot; // Logical AND set one bit to zero in the existing byte
        }
    }
}

//============================================
// printCharXxY(int xOffs, int yOffs, char c)
// xOffs = position of the left side of the character
// yOffs = position of the top of the character
// color = ON means yellow, OFF means black
// c = ASCII Character
// returns new x position
//============================================

int printChar5x8v(int xOffs, int yOffs, int color, unsigned char c)
{
    unsigned char x, y, w;
    for (x = 0; x < 5; x++)
    {
        w = font5x8v[c][x];
        for (y = 0; y < 8; y++)
        {
            if (w & 1)
                setFrameBuffer(x + xOffs, 6 - y + yOffs, color);
            w = w >> 1;
        }
    }
    return (xOffs + 6);
}

int printChar6x8v(int xOffs, int yOffs, int color, unsigned char c)
{
    unsigned char x, y, w, ctmp;
    ctmp = c - 32;
    for (x = 0; x < 6; x++)
    {
        w = font6x8v[ctmp][x];
        for (y = 0; y < 8; y++)
        {
            if (w & 1)
                setFrameBuffer(x + xOffs, 7 - y + yOffs, color);
            w = w >> 1;
        }
    }
    return (xOffs + 6);
}

int printChar6x8(int xOffs, int yOffs, int color, unsigned char c)
{
    unsigned char x, y, w;

    for (y = 0; y < 8; y++)
    {
        w = font6x8[c][y];
        for (x = 0; x < 8; x++)
        {
            if (w & 1)
                setFrameBuffer(x + xOffs, y + yOffs, color);
            w = w >> 1;
        }
    }
    return (xOffs + 7);
}

int printChar8x8(int xOffs, int yOffs, int color, unsigned char c)
{
    unsigned char x, y, w;

    for (y = 0; y < 8; y++)
    {
        w = font8x8[c][y];
        for (x = 0; x < 8; x++)
        {
            if (w & 1)
                setFrameBuffer(x + xOffs, y + yOffs, color);
            w = w >> 1;
        }
    }
    return (xOffs + 8);
}

int printChar8x12(int xOffs, int yOffs, int color, unsigned char c)
{
    unsigned int x, y, w;

    for (y = 0; y < 12; y++)
    {
        w = font8x12[c][y];
        for (x = 0; x < 8; x++)
        {
            if (w & 1)
                setFrameBuffer(x + xOffs, y + yOffs, color);
            w = w >> 1;
        }
    }
    return (xOffs + 9);
}

int printChar9x16v(int xOffs, int yOffs, int color, unsigned char c)
{
    unsigned char x, y, wL, wH, ctmp;
    ctmp = c - 32;
    if (ctmp >= 64)
        ctmp -= 32; // only big lettess allowed

    if (ctmp >= 32)
        ctmp += 32;

    for (x = 0; x < 9; x++)
    {
        wL = font9x16v[ctmp * FONT_WIDTH + x];
        wH = font9x16v[ctmp * FONT_WIDTH + FONT_WIDTH * 32 + x];
        for (y = 0; y < 8; y++)
        {
            if (wH & 1)
                setFrameBuffer(x + xOffs, 7 - y + yOffs, color);
            wH = wH >> 1;
            if (wL & 1)
                setFrameBuffer(x + xOffs, 15 - y + yOffs, color);
            wL = wL >> 1;
        }
    }
    return (xOffs + 11);
}

//============================================
// printString(int xOffs, int yOffs, char s)
// xOffs = position of the left side of the string
// yOffs = position of the top of the string
// color = ON means yellow, OFF means black
// s = string
//============================================
int printString(int xOffs, int yOffs, int color, int size, const char *s)
{
    int i, x, y;

    i = 0;
    x = xOffs;
    y = yOffs;
    while ((s[i] != '\0') && (i < 200))
    {
        switch (size)
        {
        case XSMALL:
            x = printChar5x8v(x, y, color, s[i]);
            break;
        case SMALL:
            x = printChar6x8v(x, y, color, s[i]);
            break;
        case MEDIUM:
            x = printChar8x8(x, y, color, s[i]);
            break;
        case LARGE:
            x = printChar8x12(x, y, color, s[i]);
            break;
        case XLARGE:
            x = printChar9x16v(x, y, color, s[i]);
            break;
        default:
            x = printChar6x8(x, y, color, s[i]);
        }
        i++;
    }
    return (x);
}

int hex2int(char *hex)
{
    int val = 0;
    while (*hex)
    {
        // get current character then increment
        uint8_t byte = *hex++;
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9')
            byte = byte - '0';
        else if (byte >= 'a' && byte <= 'f')
            byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <= 'F')
            byte = byte - 'A' + 10;
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}

//============================================
// printBitmap(int xOffs, int yOffs, int color, int xSize, int ySize, char *s)
// xOffs = position of the left side of the bitmap
// yOffs = position of the top of the bitmap
// color = ON means yellow, OFF means black
// xSize = horizontal size of the bitmap
// ySize = vertical sizw of the bitmap
// s = string
//============================================
int printBitmap(int xOffs, int yOffs, int color, int xSize, int ySize, const char *s)
{
    int i, x, y, xs, ys, xt, yt, xo, yo, w;
    char stmp[3];

    i = 0;

    if (xSize > 0 && xSize <= X_SIZE && ySize > 0 && ySize <= Y_PIXELS)
    {
        while ((s[i] != '\0') && (i < 200))
        {
            ys = ySize;
            yo = 0;
            while (ys > 0)
            {
                if (ys < 8)
                    yt = ys;
                else
                    yt = 8;
                for (y = 0; y < yt; y++)
                {
                    xs = xSize;
                    xo = 0;
                    while (xs > 0)
                    {
                        stmp[0] = s[i];
                        stmp[1] = s[i + 1];
                        stmp[2] = 0;
                        w = hex2int(stmp);
                        //printf("w=%0x x=%d y=%d\n", w, xo, y+yo);
                        i += 2;
                        if (xs < 8)
                            xt = xs;
                        else
                            xt = 8;
                        for (x = 0; x < xt; x++)
                        {
                            if (w & 1)
                                setFrameBuffer(xt - 1 - x + xo + xOffs, y + yo + yOffs, color);
                            w = w >> 1;
                        }
                        xs -= xt;
                        xo += xt;
                    }
                }
                ys -= yt;
                yo += yt;
            }
        }
    }
    else
        printf("error size, x %d, y %d", xSize, ySize);
    return (x);
}

//====================================================
// Draws a horizotal line at row Y
// color = BLACK   all pixels set to black
// color = YELLOW  all pixels set to yellow
//===================================================
void hLine(int y, int color)
{
    int i;

    for (i = 0; i < X_SIZE; i++)
    {
        setFrameBuffer(i, y, color);
    }
}

//====================================================
// Draws a vertical line at column X
// color = BLACK   all pixels set to black
// color = YELLOW  all pixels set to yellow
//====================================================
void vLine(int x, int color)
{
    int i;

    for (i = 0; i < Y_PIXELS; i++)
    {
        setFrameBuffer(x, i, color);
    }
}

void setCursorPos(int XPos, int YPos)
{
#ifdef _WIN32
  COORD coord;
  coord.X = XPos;
  coord.Y = YPos;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
  printf("\033[%d;%dH", YPos + 1, XPos + 1);
#endif
}

void clearScreen()
{
#ifdef _WIN32
  system("cls");
#else
  printf("\033[2J");
#endif
}

//============================================
// DEBUG ONLY
// printFrameBuffer is only used to see the
// content on the screen for debug
//============================================
void printFrameBuffer()
{
    int x, y, bitNo, maxBits;
    unsigned char w;

    maxBits = 8;
    setCursorPos(0, 0);
    for (y = 0; y < Y_SIZE; y++)
    {
        w = 1; // most right bit set
        if (y == Y_SIZE - 1)
        {
            maxBits = 8 - Y_SIZE * 8 % Y_PIXELS;
        }
        for (bitNo = 0; bitNo < maxBits; bitNo++)
        {
            for (x = 0; x < X_SIZE; x++)
            {
                if (frameBuffer[x][y] & w)
                    printf("#");
                else
                    printf(".");
            }
            w = w << 1;
            printf("\n");
        }
    }
}

//############################# Main ###############################
int main(int argc, char *argv[])
{
    int i, j;

    clearScreen();
    while (true)
    {

        clearFrameBuffer(OFF);
        i = printString(1, 0, ON, XSMALL, "ABCDEFGHIJKLM");
        i = printString(1, 5, ON, XSMALL, "noqrstuvwxyz");
        i = printString(1, 11, ON, XSMALL, "1234567890!=()");
        printFrameBuffer();
        printf("Extra Small Font 5x6 ");
        sleep(2);

        clearFrameBuffer(OFF);
        i = printString(1, 0, ON, SMALL, "ABCDEFGHIJKLM");
        i = printString(1, 8, ON, SMALL, "NOPQRSTUVWXYZ");
        printFrameBuffer();
        printf("Small Font 6x8       ");
        sleep(2);

        clearFrameBuffer(OFF);
        i = printString(1, 0, ON, SMALL, "abcefghifklm");
        i = printString(1, 8, ON, SMALL, "noqrstuvwxyz");
        printFrameBuffer();
        sleep(2);

        clearFrameBuffer(OFF);
        i = printString(1, 0, ON, MEDIUM, "ABCDEFGHIJKLM");
        i = printString(1, 8, ON, MEDIUM, "NOPQRSTUVWXYZ");
        printFrameBuffer();
        printf("Medium Font 8x8      ");
        sleep(2);

        clearFrameBuffer(OFF);
        i = printString(1, 0, ON, MEDIUM, "abcefghifklm");
        i = printString(1, 8, ON, MEDIUM, "noqrstuvwxyz");
        printFrameBuffer();
        sleep(2);

        clearFrameBuffer(OFF);
        i = printString(2, 2, ON, LARGE, "ABCDEFGHIJKLM");
        printFrameBuffer();
        printf("Large Font 8x12      ");
        sleep(2);

        clearFrameBuffer(OFF);
        i = printString(2, 2, ON, LARGE, "abcefghifklm");
        printFrameBuffer();
        sleep(2);

        clearFrameBuffer(OFF);
        i = printString(2, 0, ON, XLARGE, "ABCDEFGHIJKLM");
        printFrameBuffer();
        printf("Extra Large Font 9x16 ");
        sleep(2);

        clearFrameBuffer(OFF);
        i = printString(2, 0, ON, XLARGE, "12345678");
        printFrameBuffer();
        sleep(2);

        clearFrameBuffer(OFF);
        //i =   printBitmap(0,0,ON,8,8,"1020408001020408");
        i = printBitmap(2, 0, ON, 4, 4, "09000906");
        i = printBitmap(2, 6, ON, 4, 4, "09000609");
        i = printBitmap(2, 12, ON, 4, 4, "09000f00");
        i = printBitmap(12, 0, ON, 8, 8, "0066660081423C00");
        i = printBitmap(12, 8, ON, 8, 8, "006666003C428100");
        i = printBitmap(32, 2, ON, 8, 9, "FF81422418244281FF");
        i = printBitmap(42, 0, ON, 16, 16, "1000010000100001111122224444888811112222444488881111222244448888");
        i = printBitmap(60, 0, ON, 16, 16, "FFFF800180018001800180018001FFFF8001800180018001800180018001FFFF");
        //   vLine(0,ON);
        //   vLine(77,ON);
        //   hLine(0,ON);
        //   hLine(15,ON);
        printFrameBuffer();
        sleep(2);
    }
}
