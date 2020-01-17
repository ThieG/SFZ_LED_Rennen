void setupTrack();
void drawCountdown(int countdownStage);
void drawPlayer(struct Player *player);
void drawWinner(struct Player *winner);
void drawPixel(int Pixel, byte red, byte green, byte blue);
void drawFadeToBlack(int ledNo, byte fadeValue);
void drawMeteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay);
void drawMeteorRainStop(void);
