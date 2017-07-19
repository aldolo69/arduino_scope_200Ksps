#include "scopedisplay.h"
#include "arduino.h"



#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

//memory mapped by columns
//byte are shown vertically. from 0 to 83 map the first 8 rows
//bit 0 is at the bottom
//X0  X1  .... 83   <--bit 7
//X   X
//X   X             <--bit 0
//
//X84 X85
//X   X
//X   X
//...

extern uint8_t pcd8544_buffer[];

//terminal display position and size
int displayX = 0;
int displayY = 0;
const int displayMaxX = 13; //0..13
const int displayMaxY = 5; //0..5

byte cDisplayContrast = 50;

void displayConstrastSet(byte cDC)
{
  cDisplayContrast = cDC;
  display.setContrast(cDisplayContrast);
}
byte displayContrastGet()
{
  return cDisplayContrast;
}




//0..64 pixle for 0..1V (more or less). 10% error
//display is only 48 pixel
//1/64=0,0156v per pixel
//every 16 pixel=0,25volt
//every 8 pixel=0,125volt

//#.#.#.#.
//#.......
//........
//........
//#.#.#.#.
//#.......
//........
//........
//

void displayDrawGrid()
{
  char*addr;
  int grid[] = {
    0b0000000000110011,
    0b0000000000000001,
    0b0000000000000001,
    0b0000000000000001
  };



  for (int row = 0; row < 6; row++)
  {
    addr = pcd8544_buffer + (row * 84) ;

    for (int column = 0; column < 10; column ++)
    {
      memcpy(addr, (char*)grid, 8);
      addr += 8;
    }
    memcpy(addr, (char*)grid, 4);


  }
  displayLineX1Y1X2Y2(0, 24, 83, 24);
  displayLineX1Y1X2Y2(40, 0, 40, 47);
}



void displaySetup() {

  display.begin();
  displayConstrastSet(cDisplayContrast);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.clearDisplay();
  display.display();
}

void displayScrollUp8()
{
  int addr;

  for (int row = 0; row < 6; row++)
  {
    addr = row * 84 ;
    for (int column = 0; column < 84; column++)
    {
      pcd8544_buffer[addr] = pcd8544_buffer[addr + 84];
      addr++;
    }
  }
  addr = 84 * 5;
  for (int column = 0; column < 84; column++)
  {
    pcd8544_buffer[addr] = 0;
    addr++;
  }
}

//display a string with \n support and automatic scroll
void displayPrint(char*str)
{
  char c;
  for (int i = 0;; i++)
  {
    c = str[i];
    if (c == 0) break;
    if (c == '\n' || displayX > displayMaxX)
    {
      displayX = 0;
      if (displayY < displayMaxY) {
        displayY++;
      }
      else
      {
        displayScrollUp8();
      }
      if (c == '\n') continue;
    }

    display.drawChar(displayX * 6, displayY * 8, c, BLACK, WHITE, 1);
    displayX++;
  }
}




void displayPrintXY(char*str, int x, int y)
{
  char c;
  for (int i = 0;; i++)
  {
    c = str[i];
    if (c == 0) break;

    display.drawChar(x * 6, y * 8, c, BLACK, WHITE, 1);
    x++;
  }

}


void displayPixelXY(int x, int y)
{
  display.drawPixel(x, y, BLACK);
}

void displayLineX1Y1X2Y2(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
  display.drawLine(  x0,  y0,   x1,   y1, BLACK);
}
