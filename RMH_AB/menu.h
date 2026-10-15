#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include "globals.h"

void stateMenuIntro()
{
  if (!arduboyIntro()) return;
  globalCounter++;
  sprites.drawSelfMasked(49, 20, T_arg, 0);
  drawTwinkles(49, 20, globalCounter);
  if (globalCounter > 90)
  {
    globalCounter = 0;
    gameState = STATE_MENU_MAIN;
  }
};

void stateMenuMain()
{
  drawLz(titleLz, 896);
  sprites.drawSelfMasked(21, 56, menuMain, menuSelection - 3);        // PLAY INFO CONF JBOX, in the order of the menu states
  if (arduboy.justPressed(RIGHT_BUTTON) && (menuSelection < STATE_MENU_JBOX)) { menuSelection++; sfx(sfxMove); }
  if (arduboy.justPressed(LEFT_BUTTON) && (menuSelection > STATE_MENU_PLAY)) { menuSelection--; sfx(sfxMove); }
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) { gameState = menuSelection; sfx(sfxSelect); }
}

void stateMenuHelp()
{
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) gameState = STATE_MENU_MAIN;
}

void stateMenuInfo()
{
  drawLz(infoLz, 1024);
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) gameState = STATE_MENU_MAIN;
}

void stateMenuSoundfx()
{
  drawLz(titleLz, 896);
  sprites.drawSelfMasked(27, 56, menuOffOn, arduboy.audio.enabled());
  if (arduboy.justPressed(RIGHT_BUTTON)) arduboy.audio.on();
  if (arduboy.justPressed(LEFT_BUTTON)) arduboy.audio.off();
  if (arduboy.justPressed(A_BUTTON | B_BUTTON))
  {
    arduboy.audio.saveOnOff();
    gameState = STATE_MENU_MAIN;
    sfx(sfxSelect);
  }
}

// the jukebox: MUSIC (UP) and SNDFX (DOWN), LEFT = next number, RIGHT = previous number, B = play, A = stop (and leave when nothing plays)
byte jbRow = 0;
byte jbNr[2];                                  // MUSIC 0..8, SNDFX 0..8

// little arrows around the number at x, y (w x h pixels): v = above and below (UP / DOWN changes it), else left and right (LEFT / RIGHT changes it)
void jbArrows(byte x, byte y, byte w, byte h, bool v)
{
  byte cx = x + w / 2, cy = y + h / 2;
  if (v)
  {
    arduboy.drawPixel(cx, y - 3, WHITE);
    arduboy.drawFastHLine(cx - 1, y - 2, 3, WHITE);
    arduboy.drawFastHLine(cx - 1, y + h + 1, 3, WHITE);
    arduboy.drawPixel(cx, y + h + 2, WHITE);
  }
  else
  {
    arduboy.drawPixel(x - 3, cy, WHITE);
    arduboy.drawFastVLine(x - 2, cy - 1, 3, WHITE);
    arduboy.drawFastVLine(x + w + 1, cy - 1, 3, WHITE);
    arduboy.drawPixel(x + w + 2, cy, WHITE);
  }
}

void stateMenuJbox()
{
  drawLz(titleLz, 896);                        // the whole title picture, MUSIC and SNDFX share the bottom line
  for (byte i = 0; i < 2; i++)
  {
    byte x = 7 + 61 * i;                           // SNDFX (i = 1) at x = 68
    sprites.drawSelfMasked(x, 53, jboxWords, i);
    jbArrows(x + 40, 53, 13, 8, true);
    if (jbRow != i || (arduboy.frameCount & 16))              // the selected number blinks
    {
      sprites.drawSelfMasked(x + 40, 53, numbers, (jbNr[i] + 1) / 10);
      sprites.drawSelfMasked(x + 47, 53, numbers, (jbNr[i] + 1) % 10);
    }
  }
  byte n = jbRow ? JBOX_SFX : JBOX_MUSIC;
  if (arduboy.justPressed(LEFT_BUTTON)) jbRow = 0;
  if (arduboy.justPressed(RIGHT_BUTTON)) jbRow = 1;
  if (arduboy.justPressed(UP_BUTTON)) jbNr[jbRow] = (jbNr[jbRow] + 1) % n;
  if (arduboy.justPressed(DOWN_BUTTON)) jbNr[jbRow] = (jbNr[jbRow] + n - 1) % n;
  if (arduboy.justPressed(B_BUTTON) && arduboy.audio.enabled())
  {
    if (jbRow) sfx((const uint8_t *)pgm_read_word(&sfxList[jbNr[1]]));
    else musicPlay(jbNr[0]);
  }
  if (arduboy.justPressed(A_BUTTON))
  {
    if (musicId == MUSIC_OFF) gameState = STATE_MENU_MAIN;    // nothing playing: leave, the title tune restarts
    else musicPlay(MUSIC_OFF);
  }
}

void stateMenuPlay()
{
  drawLz(grillLz, 1024);
  sprites.drawSelfMasked(61, 23, numbers, howManyWins);
  for (byte shrimpAmount = 0; shrimpAmount < howManyWins; shrimpAmount++) sprites.drawPlusMask(5 + (shrimpAmount * 17), 35, BBQshrimp_plus_mask, 0);
  if (arduboy.justPressed(RIGHT_BUTTON | UP_BUTTON) && (howManyWins < 7)) { howManyWins++; sfx(sfxMove); }
  if (arduboy.justPressed(LEFT_BUTTON | DOWN_BUTTON) && (howManyWins > 1)) { howManyWins--; sfx(sfxMove); }
  if (arduboy.justPressed(B_BUTTON))
  {
    gameState = STATE_GAME_INIT_LEVEL;
    musicPickGameTune();
    sfx(sfxSelect);
  }
  if (arduboy.justPressed(A_BUTTON)) gameState = STATE_MENU_MAIN;
}


#endif
