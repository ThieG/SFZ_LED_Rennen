/*!
****************************************************************************************************
*
*  SFZ LED Rennen
*            
*  Das Projekt „SFZ LED Rennen“ entstand, um bei diversen Veranstaltungen, 
*  Vorführungen oder beim Tag der offenen Tür eine Attraktion anbieten zu können.
*  Das „SFZ LED Rennen“ soll Anziehungspunkt, Mitmachangebot und Türöffner 
*  für Gespräche und Interessierte sein.
*
****************************************************************************************************
*/
#include <FastLED.h>

struct Player {
  int id;  
  int buttonPin;
  int buttonLed;
  int buttonLedState;
  int buttonState;
  unsigned long startTime;
  int prevPosition;
  int position;
  int loop;
  float speed;
  bool isWinner;
  bool isOffTrack;
  bool blinking;
  CRGB::HTMLColorCode color;
  CRGB::HTMLColorCode highSpeedColor;
};
