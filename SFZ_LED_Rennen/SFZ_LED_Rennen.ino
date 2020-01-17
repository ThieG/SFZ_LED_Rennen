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
*  Das Original und die Idee stammt von hier: https://twitter.com/openledrace
*
****************************************************************************************************
*/
#include <FastLED.h>
#include "constants.h"
#include "player.h"
#include "draw.h"

/*------------------------------------------------------------------------------------------------*/
/*!
* \brief     Spieler Objekte
*/
/*------------------------------------------------------------------------------------------------*/
struct Player player1;
struct Player player2;

/*------------------------------------------------------------------------------------------------*/
/*!
* \brief     Konfiguration und Laufzeitdaten
*/
/*------------------------------------------------------------------------------------------------*/
int raceCountdown = 0;
boolean raceFinished = false;
unsigned long raceTimeWinner;
int lastStartPressed = 0;

#define RACE_WAIT_FOR_PLAYERS  (int)0
#define RACE_START_COUNTDOWN   (int)1
#define RACE_IS_RUNNING        (int)2
#define RACE_IS_FINISHED       (int)3
int RaceMode = RACE_WAIT_FOR_PLAYERS;

/*------------------------------------------------------------------------------------------------*/
/*!
* \brief     Blink Pin Konfiguration und Laufzeitdaten
*/
/*------------------------------------------------------------------------------------------------*/
int BlinkLED_Pin = START_LED_PIN;
int BlinkLED_Zustand = LOW;
unsigned long BlinkLED_AlterZeitStempel = 0;
const long BlinkLED_Intervall = 1000;


/*------------------------------------------------------------------------------------------------*/
/*!
*            Arduino Setup
*
*            Wird beim Start des Programms (entweder nach dem uebertragen auf das Board oder 
*            nach dem die Stromversorgung angeschlossen wurde) einmalig aufgerufen, 
*            um z. B. Pins als Eingang oder Ausgang zu definieren.
*/
/*------------------------------------------------------------------------------------------------*/
void setup() {
  Serial.begin(57600);

  Serial.println(F(" "));
  Serial.println(F(" ------------------------------------"));
  Serial.println(F("    _____ ______ ______ "));
  Serial.println(F("   / ____|  ____|___  / "));
  Serial.println(F("  | (___ | |__     / /  "));
  Serial.println(F("   \\___ \\|  __|   / /   "));
  Serial.println(F("   ____) | |     / /__  "));
  Serial.println(F("  |_____/|_|    /_____| "));
  Serial.println(F(" "));
  Serial.println(F(" ------------------------------------"));
  
  Serial.println(F("Info: Ready Player One..."));
  initPlayer(&player1, 1, PLAYER1_PIN, PLAYER1_LED_PIN, CRGB::Yellow, CRGB::Red);
  Serial.println(F("Info: Ready Player Two..."));
  initPlayer(&player2, 2, PLAYER2_PIN, PLAYER2_LED_PIN, CRGB::Blue, CRGB::Red);

  Serial.println(F("Info: Die Rennstrecke wird vorbereitet!"));
  setupTrack();

  pinMode(BlinkLED_Pin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(START_PIN), IRQ_StartNextRace, FALLING); 
  pinMode(START_PIN, INPUT); 

  Serial.println(F("Info: Startknopf LED Timer wird initialisiert!"));
  setupBlinkLEDTimer();
  
  Serial.println(F(" ------------------------------------"));
}


/*------------------------------------------------------------------------------------------------*/
/*!
*            Start Interrupt Service Funktion
*
*            Mit Drücken des Start Knopfs wird hier der Start Countdown angezählt!
*/
/*------------------------------------------------------------------------------------------------*/
void IRQ_StartNextRace (void){
  if (RaceMode == RACE_WAIT_FOR_PLAYERS) {
    if ((millis() - lastStartPressed) > 200) {    
      raceCountdown = 3;
      RaceMode = RACE_START_COUNTDOWN; 
      drawMeteorRainStop();
      lastStartPressed = millis();
    }
  }  
}

/*------------------------------------------------------------------------------------------------*/
/*!
*            Blink LED Interrupt Service Funktion
*
*            Diese ISR Funktion wird alle 1s aufgerufen und die blaue 
*            LED auf dem Board im Sekunden-Takt ein/aus geschalten.
*/
/*------------------------------------------------------------------------------------------------*/
ISR(TIMER1_COMPA_vect) {    
  if (BlinkLED_Zustand == LOW) {
    BlinkLED_Zustand = HIGH;  
  } else {
    BlinkLED_Zustand = LOW;  
  }
  if (RaceMode == RACE_WAIT_FOR_PLAYERS) {
    digitalWrite(BlinkLED_Pin, BlinkLED_Zustand);
  }
}

/*------------------------------------------------------------------------------------------------*/
/*!
*            Blink LED Timer Setup
*
*            Diese ISR Funktion wird der Timer fuer die Blink LED so konfiguriert, 
*            das er alle 1s aufgerufen wird.
*/
/*------------------------------------------------------------------------------------------------*/
void setupBlinkLEDTimer (void) {
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
}

/*------------------------------------------------------------------------------------------------*/
/*!
*            Arduino Main Loop
*
*            Wird durchgehend immer wieder durchlaufen, solange das Arduino-Board eingeschaltet ist.
*/
/*------------------------------------------------------------------------------------------------*/
void loop() {

  int myRed; 
  int myGreen; 
  int myBlue;
   
  switch (RaceMode) {
    case RACE_WAIT_FOR_PLAYERS:
      
      Serial.println(F("Info: Geniese die Lichtshow....")); 
      myRed   = random(0, 255);    
      myGreen = random(0, 255);
      myBlue  = random(0, 255);
      drawMeteorRain(myRed, myGreen, myBlue, 10, 64, true, 10);
      Serial.println(F("Info: Die Show ist zu Ende "));
      Serial.println(F("------------------------------------"));     
      break;
    case RACE_START_COUNTDOWN:
      
      Serial.print(F("Info: Start in "));
      Serial.println(raceCountdown);
      /* der Startknopf ist deaktiviert, daher soll die LED auch aus bleiben! */
      digitalWrite(BlinkLED_Pin, HIGH); 
      drawCountdown(raceCountdown);    
      raceCountdown--;
  
      if (raceCountdown < 0) {
        RaceMode = RACE_IS_RUNNING;
        startPlayer(&player1);
        startPlayer(&player2);
        raceCountdown = 0;
        Serial.println(F("Info: Das Rennen ist gestartet!"));
        Serial.println(F("------------------------------------"));
      } else {                
        delay(100);
      }
      break;
    case RACE_IS_RUNNING:
          
      /* Tasten abfragen, Geschwindigkeit und Position bestimmen */
      movePlayer(&player1);
      movePlayer(&player2);

      /* Aktuelle Position der LED ausgeben */
      drawPlayer(&player1);
      drawPlayer(&player2);
        
      if (isRaceFinished()) {
        struct Player winner = findWinner(player1, player2);

        /* Berechne die Bestzeit des Gewinners.... */
        unsigned long curTime = millis();
        raceTimeWinner = curTime - winner.startTime;

        /* Lass die Lichter-Fanfaren erklingen.... */
        drawWinner(&winner); 
        delay(5);       
        drawWinner(&winner); 
        
        raceFinished = true;
        RaceMode = RACE_IS_FINISHED;
               
      }
      break;
    case RACE_IS_FINISHED:
      Serial.println(F("Info: Das Rennen ist zu Ende! "));
      Serial.print(F("Info: Die Bestzeit war  "));
      Serial.print(raceTimeWinner);
      Serial.println(F("ms"));
      Serial.println(F("------------------------------------"));
      
      delay(1500);
      /* Schalte alle LEDs der Rennstrecke für die Lichtshow wieder aus!*/
      drawCountdown(0);

      /* Die LEDs der Spieler wieder einschalten */
      player1.buttonLedState = LOW;
      digitalWrite(player1.buttonLed, player1.buttonLedState);
      player2.buttonLedState = LOW;
      digitalWrite(player2.buttonLed, player2.buttonLedState);
        
      RaceMode = RACE_WAIT_FOR_PLAYERS;
      break; 
    default:
      Serial.print(F("Info: Ups, was tue ich hier?! "));
      RaceMode = RACE_WAIT_FOR_PLAYERS;
      break;       
  }

  delay(15);
}

/*------------------------------------------------------------------------------------------------*/
/*!
*            Spieler Initialisierung
*
*            Initalisiert das "Spieler" Objekt
*/
/*------------------------------------------------------------------------------------------------*/
void initPlayer(struct Player *player, int id, int pin, int led, CRGB::HTMLColorCode color, CRGB::HTMLColorCode highSpeedColor) {
  player->id = id;
  player->buttonPin = pin;
  player->buttonLed = led;
  player->buttonLedState = LOW;
  player->buttonState = LOW;
  player->prevPosition = 0;
  player->position = 0;
  player->loop = 0;
  player->speed = 0.0;
  player->isWinner = false;
  player->isOffTrack = false;
  player->blinking = false;
  player->color = color;
  player->highSpeedColor = highSpeedColor;
  
  pinMode(player->buttonLed, OUTPUT);
  digitalWrite(player->buttonLed, player->buttonLedState);
}

/*------------------------------------------------------------------------------------------------*/
/*!
*            Spieler Startvorbereitung
*
*            Bereitet das "Spieler" Objekt zum Start vor.
*/
/*------------------------------------------------------------------------------------------------*/
void startPlayer(struct Player *player) {

  player->buttonLedState = HIGH;
  player->buttonState = LOW;
  player->prevPosition = 0;
  player->position = 0;
  player->loop = 0;
  player->speed = 0.0;
  player->isWinner = false;
  player->isOffTrack = false;
  player->blinking = false;

  player->startTime = millis();

  digitalWrite(player->buttonLed, player->buttonLedState);

}

/*------------------------------------------------------------------------------------------------*/
/*!
*            Taster Check
*
*            Hier wird nachgesehen, ob der Spieler die Taste nach dem er sie gedrückt hat, 
*            ob er diese wieder losgelassen hat.
*/
/*------------------------------------------------------------------------------------------------*/
boolean buttonReleased(struct Player *player) {
  int previousState = player->buttonState;
  player->buttonState = digitalRead(player->buttonPin);

  if (previousState == HIGH && player->buttonState == LOW) {
    return true;
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/
/*!
*            Spieler Bewegung berechnen
*
*            Hier wird die Geschwindigkeit und Position des LED - Autos bestimmt.
*/
/*------------------------------------------------------------------------------------------------*/
void movePlayer(struct Player *player) {
  if (buttonReleased(player)) {
    player->speed += ACCELERATION; // acceleration

    if (player->isOffTrack) {
      player->blinking = !(player->blinking);
    }
  }

  player->speed -= player->speed * FRICTION;

  if (player->speed < 0) {
    player->speed = 0;
  } else if (player->speed >= MAX_SPEED) {
    player->speed = 0;
    player->isOffTrack = true;
  }

  player->prevPosition = player->position;
  player->position = (int)(player->position + player->speed) % NUM_LEDS;

  if (player->position < player->prevPosition) {
    player->loop += 1;
    //Serial.println("<L, " + (String)(player->id) + ", " + (String)(player->loop) + ">");
  }

  if (player->loop == MAX_LOOPS) {
    player->isWinner = true;
    //Serial.println("<W, " + (String)(player->id) + ", 0>");
  }

  //Serial.println("<P, " + (String)(player->id) + ", " + (String)(player->speed) + ", " + (String)(player->position) + ">");
}

boolean isRaceFinished() {
  return player1.isWinner || player2.isWinner;
}

/*------------------------------------------------------------------------------------------------*/
/*!
*            Finde den Gewinner
*
*            Am Ende des Rennens wird hier geschaut, wer gewonnen hat.
*/
/*------------------------------------------------------------------------------------------------*/
struct Player findWinner(struct Player player1, struct Player player2) {
  if (player1.isWinner) {
    return player1;
  } else if (player2.isWinner) {
    return player2;
  }
}
