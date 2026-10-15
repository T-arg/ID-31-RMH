#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include <Arduino.h>
#include "globals.h"

//define player & AI states (in game)
#define NORMAL_POSTURE           0
#define KICKING_POSTURE          1
#define HIT_IN_FACE_POSTURE      2

#define SUPERKICK_MAX            20

byte ballState = 0;

byte currentBallFrame = 0;
byte playerState = NORMAL_POSTURE;
byte AIstate = NORMAL_POSTURE;
byte playerAnimateTimer = 0;
byte AiAnimateTimer = 0;
boolean ballMoves = false;
byte ballTimer = 0; //for making sure ball releases after timer if player does not press "B".
byte scoreMax;
byte winlose;
byte superKickMeter ;
boolean isSuperkicking = false;

class GameObject {
  public:
    byte x, y, width, height;
    int8_t xSpeed, ySpeed;
};

GameObject player, AI, ball;



void randomizeBallDirection() {
  ball.xSpeed = 0;
  ball.ySpeed = 0;
  while (ball.xSpeed == 0) {
    ball.xSpeed = random(-3, 4);
  }
  while (ball.ySpeed == 0) {
    ball.ySpeed = random(-3, 4);
  }
}

void AI_Logic() {
  //AI IS DUMB
  byte followBall = random(0, 3);

  if (ball.xSpeed > 0 && ball.x > 20 && followBall == 0 ) { //ball is moving right and is about half screen and followBall == 0
    if (ball.y > AI.y && AI.y < HEIGHT - AI.height - 4) {
      // move down
      AI.y += AI.ySpeed;
    }
    if (ball.y < AI.y ) { // go up but only if AI feet do not go past the bottom of screen
      // move up
      AI.y -= AI.ySpeed;
    }
  }
}

void checkAnimations()
{
  if ((playerAnimateTimer > 0) && (playerState != NORMAL_POSTURE)) playerAnimateTimer --;
  else playerState = NORMAL_POSTURE;
  if ((AiAnimateTimer > 0) && (AIstate != NORMAL_POSTURE)) AiAnimateTimer --;
  else AIstate = NORMAL_POSTURE;

  sprites.drawSelfMasked(player.x, player.y, playerSprites, playerState);
  sprites.drawSelfMasked(AI.x, AI.y, playerSprites, AIstate + 3);
}

void superKickMeterAdd(byte amnt_)
{
  byte old = superKickMeter;
  superKickMeter += amnt_;
  if (superKickMeter >= SUPERKICK_MAX)
  {
    superKickMeter = SUPERKICK_MAX;
    if (old < SUPERKICK_MAX) sfx(sfxReady);                  // the meter just became full
  }
}


void checkSuperKickCollisions()
{
  Rect rect;
  Point point = {.x = ball.x + 3, .y = ball.y + 3};

  rect.x = player.x + player.width; rect.y = player.y + player.height / 2; rect.width = 20; rect.height = player.height / 2;

  if (arduboy.collide(point, rect)) {
    superKickMeter = 0;
    ball.xSpeed = 10;
    ball.ySpeed = 0;
    isSuperkicking = true;
    sfx(sfxSuper);
  }
}

void superKick()
{
  if (arduboy.justPressed(B_BUTTON))
  {

    if (superKickMeter == SUPERKICK_MAX)
    {
      playerState = KICKING_POSTURE; // always perform postore kicking for super kick when button pressed and super meter is full
      checkSuperKickCollisions();
    }
  }
}



void checkCollisions()
{

  Rect rect;
  Point point = {.x = ball.x + 3, .y = ball.y + 3};

  //check if ball hit player
  rect.x = player.x; rect.y = player.y; rect.width = 16; rect.height = 24;
  if (arduboy.collide(point, rect)) {
    if ((point.y < rect.y + 24) && (point.y > rect.y + 24 - 10)) { // if ball is on the bottom side
      //change state to kicking
      playerState = KICKING_POSTURE;
      playerAnimateTimer = 10;
      superKickMeterAdd(10);
      sfx(sfxKick);
      if (ball.xSpeed < 0) ball.xSpeed --; //if ball is going left speed up ball

      if (ball.ySpeed < 0) // if ball is going up
        ball.ySpeed *= -1;  //revers the y speed
    }


    if (point.y < rect.y + 8) { // if ball is on the top side
      //change state to hitInTheFace
      sfx(sfxFace);
      playerState = HIT_IN_FACE_POSTURE;
      playerAnimateTimer = 15;
      if (ball.ySpeed > 0) // if ball is going down
        ball.ySpeed *= -1;  //revers the y speed

    }

    if (point.x > rect.x + 10) { // if ball is on the right side
      if (ball.xSpeed < 0) { // if ball is going left

        ball.xSpeed *= -1; //reverse ball x speed
      }
    }

    if (point.x < rect.x + 5) { // if ball is on the left side
      if (ball.xSpeed > 0) // if ball is going right
        ball.xSpeed *= -1; //reverse ball x speed
    }
  }



  //check if ball hit AI
  rect.x = AI.x; rect.y = AI.y; rect.width = 16; rect.height = 24;
  if (arduboy.collide(point, rect)) {
    if (isSuperkicking) {
      isSuperkicking = false;

      randomizeBallDirection();
      if (ball.xSpeed > 0) ball.xSpeed *= -1;

    }

    if (point.y < rect.y + 20) { // if ball is on the bottom side
      //change state to kicking
      AIstate = KICKING_POSTURE;
      sfx(sfxKick);
      if (ball.xSpeed > 0) ball.xSpeed ++ ; //if ball is going right speed up ball
      if (ball.ySpeed < 0) // if ball is going up
        ball.ySpeed *= -1;  //revers the y speed
    }


    if (point.y < rect.y + 4) { // if ball is on the top side
      //change state to hitInTheFace
      sfx(sfxFace);
      AIstate = HIT_IN_FACE_POSTURE;
      if (ball.ySpeed > 0) // if ball is going down
        ball.ySpeed *= -1;  //revers the y speed

    }

    if (point.x > rect.x + 10) { // if ball is on the right side
      if (ball.xSpeed < 0) // if ball is going left
        ball.xSpeed *= -1; //reverse ball x speed
    }

    if (point.x < rect.x + 5) { // if ball is on the left side
      if (ball.xSpeed > 0) // if ball is going right
        ball.xSpeed *= -1; //reverse ball x speed
    }
  }
}

void setupGame() {
  delay(400); //delay to show how score was made
  ballMoves = false;
  ballTimer = 0;
  superKickMeter = 0;
  player.xSpeed = 3;
  player.ySpeed = 3;
  player.x = 16;
  player.width = 16;
  player.height = 24;
  player.y = HEIGHT / 2 - player.height / 2;

  AI.x = WIDTH - 16 - 16;
  AI.y = player.y;
  AI.height = 24;
  AI.width = 16;
  AI.xSpeed = player.xSpeed;
  AI.ySpeed = 1;

  ball.x = WIDTH / 2 - 4;
  ball.y = HEIGHT / 2 - 4;
  ball.xSpeed = 0;
  ball.ySpeed = 0;
  randomizeBallDirection();

  ball.width = 8;
  ball.height = 8;
}

void checkIfScored() {
  Rect rect;
  Point point;
  point.x = ball.x + 3; point.y = ball.y + 3;
  rect.width = 12; rect.height = 12;

  // the four corners: bit 0 = right side, bit 1 = bottom (the left corners are the AI's goals, the right ones are yours)
  for (byte i = 0; i < 4; i++)
  {
    rect.x = (i & 1) ? WIDTH - 12 : 0;
    rect.y = (i & 2) ? HEIGHT - 12 : 0;
    if (arduboy.collide(point, rect))
    {
      sfx(sfxScore);
      if (i & 1) playerScore ++;
      else AIScore ++;
      setupGame();
    }
  }
}

void drawScores()
{
  sprites.drawSelfMasked(48, 54, numbers, playerScore);
  sprites.drawSelfMasked(75, 54, numbers, AIScore);
}


void drawBall() {
  if (arduboy.everyXFrames(2))
  {
    currentBallFrame ++;
    if (currentBallFrame > 3) currentBallFrame = 0;
  }
  sprites.drawSelfMasked(ball.x, ball.y, shrimp, currentBallFrame);
}

void moveBall()
{
  if (ballMoves)
  {
    byte oldX = ball.x, oldY = ball.y;
    ball.x += ball.xSpeed;
    ball.y += ball.ySpeed;

    //IF BALL HITS LEFT WALL
    if (ball.x < 4)
    {
      sfx(sfxWall);
      ball.x = oldX;
      ball.xSpeed *= -1;
    }
    //IF BALL HITS RIGHT WALL
    if (ball.x + ball.width > WIDTH - 4 )
    {
      sfx(sfxWall);
      ball.x = oldX;
      if (!isSuperkicking) {
        ball.xSpeed *= -1;
      }
      else if (isSuperkicking) {
        isSuperkicking = false;
        randomizeBallDirection();
        if (ball.xSpeed > 0) ball.xSpeed *= -1;
      }
    }
    //IF BALL HITS TOP WALL
    if (ball.y < 4)
    {
      sfx(sfxWall);
      ball.y = oldY;
      ball.ySpeed *= -1;
    }
    //IF BALL HITS BOTTOM WALL
    if (ball.y + ball.height > HEIGHT )
    {
      sfx(sfxWall);
      ball.y = oldY;
      ball.ySpeed *= -1;
    }
  }

}
void checkWinLose()
{
  if (playerScore == scoreMax || AIScore == scoreMax )
  {
    winlose = random(0, 3);
    gameState = STATE_GAME_OVER;
  }
}

void drawPlayArea() {
  //draw play field
  arduboy.drawRect(0, 0, WIDTH, HEIGHT, WHITE);
  arduboy.drawFastVLine(WIDTH / 2, 0, HEIGHT, WHITE);
  arduboy.drawFastVLine(WIDTH / 2 - 28, 0, HEIGHT, WHITE);
  arduboy.drawFastVLine(WIDTH / 2 + 28, 0, HEIGHT, WHITE);
  arduboy.drawCircle(WIDTH / 2, HEIGHT / 2, 16, WHITE);

  //draw goal areas
  arduboy.fillCircle(0, 0, 12, WHITE);
  arduboy.fillCircle(0, HEIGHT, 12, WHITE);
  arduboy.fillCircle(WIDTH, HEIGHT, 12, WHITE);
  arduboy.fillCircle(WIDTH, 0, 12, WHITE);
}


#endif
