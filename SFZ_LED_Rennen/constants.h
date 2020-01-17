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

#define RACE_SOUND_IS_ENABLED 0

#define ACCELERATION 0.4   /* Beschleunigungsfaktor  */
#define FRICTION     0.03  /* Reibungsfaktor */
#define MAX_SPEED    3.6
#define MAX_LOOPS    5
#define HIGHSPEED    3

#define NUM_LEDS          300  /* 12 */
#define LED_STRIPE_PIN     4
#define START_PIN          2     /* Startknopf um das Rennn zu starten, obacht der muss an einen Interrupt Pin! */
#define START_LED_PIN      3
#define PLAYER1_PIN        6
#define PLAYER1_LED_PIN    7
#define PLAYER2_PIN        9
#define PLAYER2_LED_PIN   10

#define SPEAKER1_PIN      11
#define SPEAKER2_PIN      12
