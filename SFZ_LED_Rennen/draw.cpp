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
#include "constants.h"
#include "player.h"

void drawCylonPing(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);

boolean drawMeteorRainStopFlag;
/*------------------------------------------------------------------------------------------------*/
/*!
* \brief     LED Objekte
*/
/*------------------------------------------------------------------------------------------------*/
CRGBArray<NUM_LEDS> leds;

// private

int modulo(int x, int n) {
  return (x % n + n) % n;
}

void setTrackColor(CRGB::HTMLColorCode color) {
  leds = color;
  FastLED.show();
}

/*------------------------------------------------------------------------------------------------*/
/*!
* \brief     Initialisation der Rennstrecke
*/
/*------------------------------------------------------------------------------------------------*/
void setupTrack() {
  FastLED.addLeds < NEOPIXEL, LED_STRIPE_PIN > (leds, NUM_LEDS);
  setTrackColor(CRGB::Black);
}

void drawWinner(struct Player *winner) {
  CRGB::HTMLColorCode myColor = winner->color;
  setTrackColor(myColor);
}

void drawCountdown(int countdownStage) {
  switch (countdownStage) {
    case 1:
      //setTrackColor(CRGB::Green);
      drawCylonPing(0x00, 0xFF, 0x00, 15u, 1, 25);
      break;
    case 2:
      //setTrackColor(CRGB::Red);
      drawCylonPing(0xFF, 0x00, 0x00, 15u, 3, 25);
      break;
    case 3:
      //setTrackColor(CRGB::Blue);
      drawCylonPing(0x00, 0x00, 0xFF, 15u, 5, 25);
      break;
    default:
      setTrackColor(CRGB::Black);
      break;
  }
}

void drawOnTrack(int position, int length, CRGB::HTMLColorCode color) {
  for (int j = 0; j < length; j++) {
    int index = modulo(position - j, NUM_LEDS);
    leds[index] = color;
  }
}

CRGB::HTMLColorCode playerColor(struct Player *player) {
   if (player->speed > HIGHSPEED) {
    return player->highSpeedColor;
  } else {
    return player->color;
  }
}

boolean ledsAreBlack(int index, int length) {
  boolean res = true;

  for (int i = 0; i < length; i++) {
    int idx = modulo(index - i, NUM_LEDS);
    res |= (leds[idx].r == 0 && leds[idx].g == 0 && leds[idx].b == 0);
  }
  
  return res;
}

// used by meteorrain
void drawFadeToBlack(int ledNo, byte fadeValue) {
   // FastLED
   leds[ledNo].fadeToBlackBy(fadeValue);
}

// Set a LED color (not yet visible)
void drawPixel(int Pixel, byte red, byte green, byte blue) {
   leds[Pixel].setRGB(red, green, blue);
}

void drawMeteorRainStop (void) {
  drawMeteorRainStopFlag = true;
}

typedef struct RunTimeObjData {
    int timeStampStart;
    int durationMax;

} RunTimeObjData;
RunTimeObjData TS_Fktn[5];

void TS_RunTimeMeasStart (int id) {
  TS_Fktn[id].timeStampStart = millis();  
}

void TS_RunTimeMeasStop (int id) {
  int durationCur = millis() - TS_Fktn[id].timeStampStart;
  if (durationCur > TS_Fktn[id].durationMax) {
    TS_Fktn[id].durationMax = durationCur;
  }
}

// https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
void drawMeteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  /* */
  drawMeteorRainStopFlag = false;

  TS_RunTimeMeasStart(0);
  setTrackColor(CRGB::Black);                                 /* 10ms */
  TS_RunTimeMeasStop(0);
  
  for(int i = 0; i < (NUM_LEDS); i++) {
    TS_RunTimeMeasStart(1);
    // fade brightness all LEDs one step
    for(int j=0; j<NUM_LEDS; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        drawFadeToBlack(j, meteorTrailDecay );        
      }
    }
    TS_RunTimeMeasStop(1);
    TS_RunTimeMeasStart(2);   
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <NUM_LEDS) && (i-j>=0) ) {
        drawPixel(i-j, red, green, blue);
      }
    }
    TS_RunTimeMeasStop(2);
    TS_RunTimeMeasStart(3);
    FastLED.show();
    TS_RunTimeMeasStop(3);    
    delay(SpeedDelay);
    
    if (drawMeteorRainStopFlag) {
      i = (NUM_LEDS + NUM_LEDS + 1);
    }
  }

  Serial.print(F("Info: setTrackColor(CRGB::Black) : "));
  Serial.println(TS_Fktn[0].durationMax);
  Serial.print(F("Info: drawFadeToBlack()          : "));
  Serial.println(TS_Fktn[1].durationMax);
  Serial.print(F("Info: drawPixel()                : "));
  Serial.println(TS_Fktn[2].durationMax);
  Serial.print(F("Info: FastLED.show()             : "));
  Serial.println(TS_Fktn[3].durationMax);
  Serial.println(F("------------------------------------")); 
  setTrackColor(CRGB::Black);
}

void drawCylonPing(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay){
    
  //for(int i = 0; i < NUM_LEDS-EyeSize-2; i++) {
  for(int i = 0; i < (75 - EyeSize - 2); i++) {
    setTrackColor(CRGB::Black);
    drawPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      drawPixel(i+j, red, green, blue);
    }
    drawPixel(i+EyeSize+1, red/10, green/10, blue/10);
    FastLED.show();
    delay(SpeedDelay);
  }
  
  delay(ReturnDelay);

  for(int i = (75-EyeSize-2); i > 0; i--) {
    setTrackColor(CRGB::Black);
    drawPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      drawPixel(i+j, red, green, blue);
    }
    drawPixel(i+EyeSize+1, red/10, green/10, blue/10);
    FastLED.show();
    delay(SpeedDelay);
  }

}

void drawPlayer(struct Player *player) {
  
  if (player->isOffTrack) {    
    // draw an explosion
    CRGB::HTMLColorCode offTrackColor = player->blinking ? player->color : CRGB::Black;

    drawOnTrack(player->position + 1, 1, offTrackColor);
    drawOnTrack(player->position - player->loop - 1, 1, offTrackColor);

    if (player->speed >= 1 && !player->blinking) {
      player->isOffTrack = false;
      // clear the back of the explosion
      drawOnTrack(player->prevPosition - player->loop - 1, 1, CRGB::Black);
    }

    FastLED.show();
  }
    
  if (player->position != player->prevPosition || (player->position == 0 && player->loop == 0) || ledsAreBlack(player->position, player->loop)) {
    // draw the player
    drawOnTrack(player->prevPosition, player->loop + 1, CRGB::Black);
    drawOnTrack(player->position, player->loop + 1, playerColor(player));

    FastLED.show();
  }
}
