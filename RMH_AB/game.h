#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include "globals.h"
#include "playfield.h"

void stateGameInitLevel()
{
  playerScore = 0;
  AIScore = 0;
  scoreMax = howManyWins;
  setupGame();
  gameState = STATE_GAME_PLAYING ;
};



void stateGamePlaying()
{

  moveBall();
  AI_Logic();
  superKick();
  checkIfScored();
  checkCollisions();
  drawPlayArea();
  drawScores();
  checkAnimations();
  drawBall();
  checkWinLose();
  if (arduboy.everyXFrames(2))
  {
    if (!ballMoves)
    {
      ballTimer ++;
      if (ballTimer == 60)
      {
        ballMoves = true;
        ballTimer = 0;
      }
    }
  }
  if (arduboy.pressed(RIGHT_BUTTON) && (player.x < 20)) player.x += player.xSpeed;
  if (arduboy.pressed(LEFT_BUTTON) && (player.x > 4)) player.x -= player.xSpeed;
  if (arduboy.pressed(UP_BUTTON) && (player.y > 4)) player.y -= player.ySpeed;
  if (arduboy.pressed(DOWN_BUTTON) && (player.y + 24 < HEIGHT - 4)) player.y += player.ySpeed;
  if (arduboy.justPressed(B_BUTTON))ballMoves = true;


  if (arduboy.justPressed(A_BUTTON))
  {
    gameState = STATE_GAME_PAUSE;
    sfx(sfxPause);
  }

};

void stateGamePause()
{
  drawLz(pauseLz, 1024);
  if (arduboy.justPressed(A_BUTTON))
  {
    gameState = STATE_GAME_PLAYING;
    sfx(sfxPause);
  }
};

const uint8_t *const endScreens[] PROGMEM = { fishWomanLz, fishCompleteLz, fishMermaidLz, fishStickLz };

void stateGameOver()
{
  // the fish sticks if the AI won, else one of the three happy endings
  drawLz((const uint8_t *)pgm_read_word(&endScreens[AIScore > playerScore ? 3 : winlose]), 1024);
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) gameState = STATE_MENU_MAIN;
};


#endif
