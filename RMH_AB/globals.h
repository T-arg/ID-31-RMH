#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Arduboy2.h>
#include "ATMconfig.h"
#include <ATMlib.h>
#include "bitmaps.h"

//define menu states (on main menu)
#define STATE_MENU_INTRO             0
#define STATE_MENU_MAIN              1
#define STATE_MENU_HELP              2
#define STATE_MENU_PLAY              3
#define STATE_MENU_INFO              4
#define STATE_MENU_SOUNDFX           5
#define STATE_MENU_JBOX              6

//define game states (on main menu)
#define STATE_GAME_INIT_LEVEL        7
#define STATE_GAME_PLAYING           8
#define STATE_GAME_PAUSE             9
#define STATE_GAME_OVER              10

#define HOW_MANY_WINS_AT_START       4

Arduboy2Base arduboy;
Sprites sprites;
ATMsynth ATM;

//determines the state of the game
byte gameState = STATE_MENU_INTRO;   // start the game with the TEAM a.r.g. logo
//generic variable to store menuSelection (on screens)
byte menuSelection = STATE_MENU_PLAY; // PLAY menu item is pre-selected
byte globalCounter = 0;

// These are all getting a value in STATE_GAME_INIT
byte howManyWins = HOW_MANY_WINS_AT_START;
byte playerScore = 0;
byte AIScore = 0;


// unpack a LZ packed picture (see tools/rmh/rmh_ui.py) straight into the screen buffer, n bytes (128 per page):
// 0..127 = that many literal bytes follow, 128..255 = copy (n - 128 + 3) bytes from d bytes back (d is the next byte)
void drawLz(const uint8_t *p, uint16_t n)
{
  uint8_t *b = arduboy.getBuffer();
  for (uint16_t i = 0; i < n;)
  {
    uint8_t h = pgm_read_byte(p++), l = h & 127;
    if (h & 128)
    {
      uint8_t d = pgm_read_byte(p++);
      for (l += 3; l--; i++) b[i] = b[i - d];
    }
    else while (l--) b[i++] = pgm_read_byte(p++);
  }
}

#include "arduboy_intro.h"
#include "music.h"

#endif
