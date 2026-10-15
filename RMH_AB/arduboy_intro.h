// ARDUBOY intro + T-arg logo twinkles (self-contained kit)
// before including: #define INTRO_DIV 2 for a 60 fps game (default 1 = 30 fps). Needs arduboy, sprites and ATM.
// needs in ATMconfig.h: ATM_FX_CUE, ATM_FUNC_CUE and ATM_OCTAVE_2 NOT disabled
#ifndef ARDUBOY_INTRO_H
#define ARDUBOY_INTRO_H

#ifndef INTRO_DIV
#define INTRO_DIV 1
#endif
#define INTRO_LETTERS 7
#define INTRO_SLIDE 7      // + 2 bounce frames
#define INTRO_BOUNCE 2
#define INTRO_LATE 12
#define INTRO_HOLD 30
#define INTRO_X 36
#define INTRO_Y 28

ATM_SFX_TRACK(sfxIntro, ATM_SL_VOL((uint8_t)-4), ATM_DELAY(8), ATM_VOL(48), ATM_NOTE_E2, ATM_CUE(1), ATM_DELAY(8), ATM_VOL(48), ATM_NOTE_C2, ATM_CUE(2), ATM_DELAY(8), ATM_VOL(48), ATM_NOTE_G2, ATM_CUE(3), ATM_DELAY(8), ATM_VOL(48), ATM_NOTE_D2, ATM_CUE(4), ATM_DELAY(8), ATM_VOL(48), ATM_NOTE_A2, ATM_CUE(5), ATM_DELAY(8), ATM_VOL(48), ATM_NOTE_F2, ATM_CUE(6), ATM_DELAY(8), ATM_VOL(48), ATM_NOTE_B2, ATM_CUE(7), ATM_DELAY(14));

const unsigned char PROGMEM arduboyLetters[] = { 8, 8,
  0x38,0x7d,0x7d,0x75,0x75,0x7f,0x7e,0x00, 0x7f,0x7f,0x7f,0x0d,0x0d,0x7f,0x76,0x00,
  0x7f,0x7f,0x7f,0x79,0x79,0x7f,0x3e,0x00, 0x3f,0x7f,0x7f,0x78,0x78,0x7f,0x3f,0x00,
  0x7f,0x7f,0x7f,0x75,0x75,0x7f,0x3a,0x00, 0x3e,0x7f,0x7f,0x79,0x79,0x7f,0x3e,0x00,
  0x4f,0x5f,0x5f,0x58,0x78,0x7f,0x3f,0x00 };

byte introLetter = 0, introFrame = 0, introDiv = 0;

// call every frame while the intro state runs; true when the intro is over (all letters shown and held, or skipped with A/B)
bool arduboyIntro()
{
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) introLetter = INTRO_LETTERS + 1;
  if (introLetter > INTRO_LETTERS) return true;
  if (introLetter == 0 && introFrame == 0 && introDiv == 0 && arduboy.audio.enabled()) ATM.playSfx(sfxIntro, 1);
  for (byte i = 0; i <= introLetter && i < INTRO_LETTERS; i++)
  {
    int x = INTRO_X + i * 8;
    if (i == introLetter && introFrame < INTRO_SLIDE + 2)
    {
      int t = x - INTRO_BOUNCE;
      x = (introFrame < INTRO_SLIDE) ? t + ((127 - t) * (INTRO_SLIDE - introFrame)) / INTRO_SLIDE : t + introFrame - INTRO_SLIDE;
    }
    sprites.drawSelfMasked(x, INTRO_Y, arduboyLetters, i);
  }
  if (++introDiv < INTRO_DIV) return false;
  introDiv = 0;
  introFrame++;
  if (introLetter < INTRO_LETTERS)
  { if (ATM.check() || introFrame > INTRO_LATE) { introLetter++; introFrame = 0; } }
  else if (introFrame > INTRO_HOLD) introLetter++;
  return false;
}

// 5 twinkling stars around the logo drawn at (lx, ly); t = a counter that goes up every frame
#define TWINKLES 5
const int8_t PROGMEM twinkleXY[] = { 25, 20, 4,  -4, 6, 12,  35, -5, 17,  13, 30, 8,  4, -10, 1 };
void drawTwinkles(int lx, int ly, uint16_t t)
{
  for (byte i = 0; i < TWINKLES; i++)
  {
    byte p = (t / INTRO_DIV + pgm_read_byte(&twinkleXY[i * 3 + 2])) % 20;
    if (p > 10) p = 20 - p;
    byte arm = (p + 1) / 4;                         // 0, 1 or 2: the arms are at most 2 pixels long (a 5x5 star)
    if (p)
    {
      int x = lx + (int8_t)pgm_read_byte(&twinkleXY[i * 3]), y = ly + (int8_t)pgm_read_byte(&twinkleXY[i * 3 + 1]);
      arduboy.drawFastHLine(x - arm, y, arm * 2 + 1, WHITE);
      arduboy.drawFastVLine(x, y - arm, arm * 2 + 1, WHITE);
    }
  }
}

#endif
