
#include <stdlib.h>
#include "scopekeyboard.h"
#include "scopedisplay.h"
#include "scopemenu.h"

volatile int iScopeCounter = 0;   //how many samples in memory?
volatile char cScopeDivider = 1;//1,2,5,10. keep 1 sample every...
volatile char cScopeDividerCounter = 0;

#define POINTS (LCDWIDTH+(LCDWIDTH/2)) //sample memory size

unsigned char cScopeBuffer[POINTS];//sample memory

char cScopeDisplayed = 0;//1 when saples ready to be displayed
unsigned char cScopeDisplayBuffer[LCDWIDTH];//samples to be displayed

char cScopeTrigger = 'U'; //'U'p , 'D'own,'N'one,Abobe/Below (Threashould)
char cScopeTriggerThreashould = 23; //0..64 23=0volt

char cScopeScale = 0; //adc0=x10 adc1=x1 adc2=x0.1


//adc clock=cpu clock divided by 2...128
//do not use a > 4mhz clock, or PS_4 on 16mhz clock
const unsigned char PS_2   = 0b00000001;
const unsigned char PS_4   = 0b00000010;
const unsigned char PS_8   = 0b00000011;
const unsigned char PS_16  = 0b00000100;
const unsigned char PS_32  = 0b00000101;
const unsigned char PS_64  = 0b00000110;
const unsigned char PS_128 = 0b00000111;



ISR(ADC_vect)                           //ADC interrupt. continuosly running
{
  if (iScopeCounter == POINTS) return;  //buffer full?

  //sample once in a while, every iScopeDivider samples 1,2,5,10....
  cScopeDividerCounter++;
  if (cScopeDividerCounter < cScopeDivider) return;

  cScopeDividerCounter = 0;
  cScopeBuffer[iScopeCounter] = (ADCH  >> 2);//6 bits precision 0..64
  iScopeCounter++;
}




void setup() {

  tone(2, 5000);              //square wave for the negative voltage generator

  keyboardSetup();            //initialize keyboard
  displaySetup();             //initialize display
  menuSetup();                //initialize menu

  scopeInputSet( cScopeScale);//initialize sapling
}


//select adc input according to scaling
//adc0=x10
//adc1=x1
//adc2=x0.1
void scopeInputSet(char cScale)
{
  switch (cScopeScale)
  {
    case 2://x10
      ADMUX = 0b11000000 +  // use vref=1.1 internal
              0b00000000 +  // use adc0
              0b00100000;   // left justifieds bits. read only most significative one
      break;
    case 1://x1
      ADMUX = 0b11000000 +  // use vref=1.1 internal
              0b00000001 +  // use adc1
              0b00100000;   // left justifieds bits. read only most significative one
      break;
    case 0://x0.1
      ADMUX = 0b11000000 +  // use vref=1.1 internal
              0b00000010 +  // use adc2
              0b00100000;   // left justifieds bits. read only most significative one
      break;
  }

  DIDR0 = 0b00000111;   // turn off the digital input for adc0 1 and 2

  ADCSRA =  PS_4 +  //prescaler /4=4mhz for 16mhz cpu
            (1 << ADEN) +   // Enable ADC
            (1 << ADATE) +  // Enable auto-triggering
            (1 << ADIE) +   // Enable ADC Interrupt
            (1 << ADSC);    // Start A2D Conversions
}




void loop() {
  static unsigned long lNextUpdateTime = 0;
  unsigned   long lNow = millis();


  int iKey = keyboardGetButton(); //check keyboard
  if (iKey != 0) menuKey(iKey);   //key pressed: drive menu


  if (lNow > lNextUpdateTime)     //time to update the display
  {
    lNextUpdateTime = lNow + 200; //next video update in no less then 200ms.
    //maybe more if performing something slow

    if ( cScopeDisplayed == 1          //waiting for display?
         && iScopeCounter == POINTS )
    {
      iScopeCounter = 0;            //reset sample buffer and restart sampling
    }

    cScopeDisplayed = 0;          //data displayed
    displayDrawGrid();            //clear display and draw display grid

    //draw trigger level
    int iYTrigger =  cScopeTriggerThreashould;
    displayPixelXY(1, iYTrigger);
    iYTrigger++;
    displayPixelXY(2, iYTrigger);
    displayPixelXY(1, iYTrigger);
    iYTrigger++;
    displayPixelXY(1, iYTrigger);




    for (int i = 1 ; i < LCDWIDTH; i++)
    {
      int y1 = cScopeDisplayBuffer[ i - 1];
      int y2 = cScopeDisplayBuffer[ i];
      //keep trace inside video
      if (y1 >= LCDHEIGHT)
      {
        y1 = LCDHEIGHT - 1;
      }
      if (y2 >= LCDHEIGHT)
      {
        y2 = LCDHEIGHT - 1;
      }
      displayLineX1Y1X2Y2(i - 1, y1, i, y2);
    }

    menuKey(0);                   //update menu
    display.display();            //show display
  }







  if (iScopeCounter == POINTS &&  //buffer filled?
      cScopeDisplayed == 0)       //diplay not to be redraw?
  {
    cScopeDisplayed = 1;          //enable a display redraw


    //check sample for trigger
    int iScopeDisplayIndex = 0;
    int iCurrent = cScopeBuffer[0];
    if (cScopeTrigger == 'N')     //no trigger
    {
      iScopeDisplayIndex = 0;
    }
    else
    {
      for (; iScopeDisplayIndex < POINTS - LCDWIDTH; iScopeDisplayIndex++)
      {
        if (cScopeTrigger == 'B')
        {
          if (cScopeBuffer[iScopeDisplayIndex] > cScopeTriggerThreashould)
          { //below then
            break;
          }
          continue;
        }
        if (cScopeTrigger == 'A')
        {
          if (cScopeBuffer[iScopeDisplayIndex] < cScopeTriggerThreashould)
          { //higher then
            break;
          }
          continue;
        }

        if (cScopeTrigger == 'U')
        { //raising trigger
          if (iCurrent >= cScopeTriggerThreashould && cScopeBuffer[iScopeDisplayIndex] < cScopeTriggerThreashould)
          {
            break;//trigger found. exit
          }
        }
        if (cScopeTrigger == 'D')
        { //falling trigger
          if (iCurrent <= cScopeTriggerThreashould && cScopeBuffer[iScopeDisplayIndex] > cScopeTriggerThreashould)
          { break;//trigger found. exit
          }
        }
        iCurrent = cScopeBuffer[iScopeDisplayIndex];
        continue;
      }
    }

//copy samples to display memory
    memcpy(cScopeDisplayBuffer, cScopeBuffer + iScopeDisplayIndex, LCDWIDTH);

    iScopeCounter = 0;            //reset sample buffer

  }


}
