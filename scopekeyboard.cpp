#include "scopekeyboard.h"
#include "arduino.h"


int last_key_confirmed = 0;




void keyboardSetup() {
  // initialize the pushbutton pin as an input:
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonLeft, INPUT_PULLUP);
  pinMode(buttonRight, INPUT_PULLUP);
  pinMode(buttonCenter, INPUT_PULLUP);
}


//is a key pressed?
void keyboardCheck()
{

  int presentKeyPressed = 0;


  if (digitalRead(buttonLeft) == 0) presentKeyPressed = buttonLeft;
  if (digitalRead(buttonRight) == 0) presentKeyPressed = buttonRight;
  if (digitalRead(buttonUp) == 0) presentKeyPressed = buttonUp;
  if (digitalRead(buttonDown) == 0) presentKeyPressed = buttonDown;
  if (digitalRead(buttonCenter) == 0) presentKeyPressed = buttonCenter;


  last_key_confirmed = presentKeyPressed;

}


int keyboardGetButton()
{
  keyboardCheck();

  int key = last_key_confirmed;
  last_key_confirmed = 0;
  return key;
}




