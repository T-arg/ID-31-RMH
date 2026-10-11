/*
 REVERSE MARMAID HOCKEY: https://t-arg.github.io/ID-31-RMH/

 Arduboy version 1.4 : https://github.com/T-arg/ID-31-RMH
 
 MADE by TEAM a.r.g. : https://t-arg.github.io/

 2015 - DRAGULA96 - ART WORK: JO3RI & UNCLESPORKY

 Game License: MIT : https://opensource.org/licenses/MIT

 */


//determine the game
#define GAME_ID 31

#include "globals.h"
#include "menu.h"
#include "game.h"
#include "playfield.h"


typedef void (*FunctionPointer) ();

const FunctionPointer PROGMEM mainGameLoop[] = {
  stateMenuIntro,
  stateMenuMain,
  stateMenuHelp,
  stateMenuPlay,
  stateMenuInfo,
  stateMenuSoundfx,
  stateGameInitLevel,
  stateGamePlaying,
  stateGamePause,
  stateGameOver,
};



void setup()
{
  arduboy.begin();
  arduboy.setFrameRate(30);
  arduboy.initRandomSeed();
}


void loop()
{
  if (!(arduboy.nextFrame())) return;
  arduboy.pollButtons();
  arduboy.clear();
  ((FunctionPointer) pgm_read_word (&mainGameLoop[gameState]))();
  arduboy.display();
}



