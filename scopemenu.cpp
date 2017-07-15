#include "scopekeyboard.h"
#include "scopemenu.h"
#include "arduino.h"
#include "scopedisplay.h"
#include "scopemain.h"

int menuRun(int);
int menuMain(int);
int menuTrigger(int);
int menuContrast(int);


int iMenuIndex = 0;
int iMenuSize = 4;
int(*menuEntries[])(int) = {menuRun, menuMain, menuTrigger,  menuContrast};


void menuExec(int iKey)
{
  int iResult = menuEntries[iMenuIndex](iKey);
  if (iResult == 1) //next
  {
    iMenuIndex++;
    if (iMenuIndex == iMenuSize)
    {
      iMenuIndex = 0;
    }
  }
}





//input=key or 0=nokey=redraw menu
//0.....6....123
//______________
//         U/D V
//         L/R T
int menuMain(int iKey)
{
  switch (iKey)
  {
    case 0://draw menu

      displayPrintXY("V/div", 9, 0);
      switch (cScopeScale)
      {
        case 0:
          displayPrintXY("12.5mV", 8, 1);
          break;
        case 1:
          displayPrintXY("125mV", 8, 1);
          break;
        case 2:
          displayPrintXY("1.25V", 8, 1);
          break;
      }
      displayPrintXY("T/Div", 9, 2);
      switch (cScopeDivider)
      {
        case 1:
          displayPrintXY(".05mS", 9, 3);
          break;
        case 2:
          displayPrintXY(".1mS", 10, 3);
          break;
        case 5:
          displayPrintXY(".25mS", 9, 3);
          break;
        case 10:
          displayPrintXY(".5mS", 10, 3);
          break;
      }

      break;


    case buttonLeft:
      switch (cScopeDivider)
      {
        case 1:
          break;
        case 2: cScopeDivider = 1;
          break;
        case 5: cScopeDivider = 2;
          break;
        case 10: cScopeDivider = 5;
          break;
      }
      break;
    case buttonRight:
      switch (cScopeDivider)
      {
        case 1: cScopeDivider = 2;
          break;
        case 2: cScopeDivider = 5;
          break;
        case 5: cScopeDivider = 10;
          break;
        case 10:
          break;
      }
      break;



    case buttonUp:
      if (cScopeScale < 2) cScopeScale++;
      scopeInputSet(cScopeScale);
      break;
    case buttonDown:
      if (cScopeScale > 0) cScopeScale--;
      scopeInputSet(cScopeScale);
      break;

    case buttonCenter: //exit
      return 1;
  }
  return 0;
}


//according to scale...
//25pixel=0convertPixelToVoltv
void convertPixelToVolt(char cVal, char*cVolt)
{

  int iVal;
  char cSign;

  char digits[3];
  iVal = cVal;

  if (iVal <= 23)
  {
    cSign = '+';

    iVal = 23 - iVal;
  }
  else
  {
    cSign = '-';
    iVal = iVal - 23;

  }

  iVal = (125 * iVal) / 8;



  switch (cScopeScale)
  {
    case 0:

      sprintf(cVolt, "%c%i.%01imV", cSign, iVal / 10, iVal % 10);
      //          displayPrintXY("12.5mV", 8, 1);
      break;
    case 1:

      sprintf(cVolt, "%c%imV", cSign, iVal);
      //         displayPrintXY("125mV", 8, 1);
      break;
    case 2:
      sprintf(cVolt, "%c%i.%02iV", cSign, iVal / 100, iVal % 100);
      //         displayPrintXY("1.25V", 8, 1);
      break;
  }




}

int menuTrigger(int iKey)
{
  char sTrigger[11];
  char sVolt[7];//-x.xxV o -xxxmV or -xx.xmV

  switch (iKey)
  {
    case 0://draw menu

      displayPrintXY("Trigger", 7, 0);
      switch (cScopeTrigger)
      {
        case 'N':
          displayPrintXY("None", 10, 1);
          break;
        case 'U':
          displayPrintXY("Up", 12, 1);
          convertPixelToVolt(cScopeTriggerThreashould, sTrigger);
          displayPrintXY(sTrigger, 7, 2);
          break;
        case 'D':
          displayPrintXY("Down", 10, 1);
          convertPixelToVolt(cScopeTriggerThreashould, sTrigger);
          displayPrintXY(sTrigger, 7, 2);
          break;
        case 'A':
          displayPrintXY("Above", 9, 1);
          //sprintf(sTrigger, "%u", cScopeTriggerThreashould);
          convertPixelToVolt(cScopeTriggerThreashould, sTrigger);
          displayPrintXY(sTrigger, 7, 2);
          break;
        case 'B':
          displayPrintXY("Below", 9, 1);
          //sprintf(sTrigger, "%u", cScopeTriggerThreashould);
          convertPixelToVolt(cScopeTriggerThreashould, sTrigger);
          displayPrintXY(sTrigger, 7, 2);
          break;
      }

      break;


    case buttonRight:
      if (cScopeTrigger == 'A' || cScopeTrigger == 'B' || cScopeTrigger == 'U' || cScopeTrigger == 'D')
      {
        if (cScopeTriggerThreashould > 0)cScopeTriggerThreashould--;
      }
      break;
    case buttonLeft:
      if (cScopeTrigger == 'A' || cScopeTrigger == 'B' || cScopeTrigger == 'U' || cScopeTrigger == 'D')
      {
        if (cScopeTriggerThreashould < 48)cScopeTriggerThreashould++;
      }
      break;



    case buttonUp:
      switch (cScopeTrigger)
      {
        case 'U':
          break;
        case 'D': cScopeTrigger = 'U';
          break;
        case 'N': cScopeTrigger = 'D';
          break;
        case 'A': cScopeTrigger = 'N';
          break;
        case 'B': cScopeTrigger = 'A';
          break;
      }
      break;
    case buttonDown:
      switch (cScopeTrigger)
      {
        case 'U': cScopeTrigger = 'D';
          break;
        case 'D': cScopeTrigger = 'N';
          break;
        case 'N': cScopeTrigger = 'A';
          break;
        case 'A': cScopeTrigger = 'B';
          break;
        case 'B':
          break;
      }
      break;
    case buttonCenter: //exit
      return 1;
  }
  return 0;
}




//input=key or 0=nokey=redraw menu
//0.....6....123
//______________
//      Contrast
//           U/D
int menuContrast(int iKey)
{
  byte cDC = displayContrastGet();
  char sDC[4];
  switch (iKey)
  {
    case 0://draw menu

      displayPrintXY("Contrast", 6, 0);
      sprintf(sDC, "%u", cDC);
      displayPrintXY(sDC, 11, 1);
      break;

    case buttonUp:
      if (cDC < 150) cDC++;
      displayConstrastSet(cDC);
      break;
    case buttonDown:
      if (cDC > 20) cDC--;
      displayConstrastSet(cDC);
      break;
    case buttonCenter: //exit
      return 1;
  }
  return 0;
}

int menuRun(int iKey)
{
  switch (iKey)
  {
    case 0://draw menu
      displayPrintXY("Run...", 8, 0);
      break;
    case buttonUp:
      break;
    case buttonDown:
      break;
    case buttonCenter: //exit
      return 1;
  }
  return 0;
}





void menuSetup()
{
}


void menuKey(int iKey)
{
  menuExec(iKey);
}





