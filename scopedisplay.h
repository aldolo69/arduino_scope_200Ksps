
#ifndef _scopedisplay
#define _scopedisplay
#include "arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>


extern void displaySetup(void);
extern void displayPrint(char*);
extern void displayPrintXY(char*str,int x,int y);
extern void displayPixelXY(int x,int y);
void displayLineX1Y1X2Y2(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
extern void displayConstrastSet(byte cDC);
extern byte displayContrastGet();
extern void displayDrawGrid();


extern Adafruit_PCD8544 display;

#endif


 

 
