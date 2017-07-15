
#ifndef _scopemain
#define _scopemain

#define _debug 


#ifdef _debug
//insert here...
#endif


extern volatile char cScopeDivider;//1,2,5,10
extern char cScopeTrigger; //'U'p , 'D'own,'N'one,Over/Under (Threashould)
extern char cScopeTriggerThreashould; //0..64
extern char cScopeScale; //adc0=x10 adc1=x1 adc2=x0.1

void scopeInputSet(char cScale);

#endif
