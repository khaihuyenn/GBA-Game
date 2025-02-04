#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"
/* TODO: */
// Include any header files for title screen or exit
// screen images generated by nin10kit. Example for the provided garbage
// image:
// #include "images/garbage.h"
#include "images/pong1.h"
#include "images/win.h"
#include "images/lose.h"

/* TODO: */
// Add any additional states you need for your app. You are not requried to use
// these specific provided states.
enum gba_state {
  START,
  PLAY,
  WIN,
  LOSE,
};

void initialize (struct player *pP, struct ball *bP, struct player *eP, int *playerScore, int *enemyScore) {
  pP->row = 80;
  pP->col = 20;
  pP->width = 8;
  pP->height = 25;

  bP->row = 80;
  bP->col = 120;
  bP->size = 5;
  bP->velRow = 1;
  bP->velCol = 1;

  eP->row = 80;
  eP->col = WIDTH - 20;
  eP->width = 8;
  eP->height = 25;

  *playerScore = 0;
  *enemyScore = 0;
}



int main(void) {
  /* TODO: */
  // Manipulate REG_DISPCNT here to set Mode 3. //
  REG_DISPCNT = MODE3 | BG2_ENABLE;
  // Save current and previous state of button input.
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  // Load initial application state
  struct player player;
  struct ball ball;
  struct player enemy;
  int playerScore;
  int enemyScore;

  initialize(&player, &ball, &enemy, &playerScore, &enemyScore);

  enum gba_state state = START;


  int textY = 70;
  //int dy = 1;

  void moveText(int *textY);      

  void drawScores(int playerScore, int enemyScore);
  

  drawFullScreenImageDMA(pong1);

  //drawString(140, 70, "Press Enter to Play", WHITE);

  while (1) {
    currentButtons = BUTTONS; // Load the current state of the buttons

    /* TODO: */
    // Manipulate the state machine below as needed //
    // NOTE: Call waitForVBlank() before you draw
    waitForVBlank();

    if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
          drawFullScreenImageDMA(pong1);
          state = START;
          initialize(&player, &ball, &enemy, &playerScore, &enemyScore);
    }

    switch (state) {
      case START:
        drawRectDMA(140, textY, 114, 8, BLACK);

        drawString(140, textY, "Press Enter to Play", WHITE);

        moveText(&textY);

        //drawRectDMA(140, textY, 6, 8, BLACK);
        if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
          state = PLAY;
          fillScreenDMA(BLACK);
          //break;
        }
        break;
      case PLAY:
        //////////////////// ERASING PLAYER BELOW ////////////////////////
        drawRectDMA(player.row, player.col, player.width, player.height, BLACK);
        drawRectDMA(enemy.row, enemy.col, enemy.width, enemy.height, BLACK);
        drawRectDMA(ball.row, ball.col, ball.size, ball.size, BLACK);

        if (vBlankCounter % 600 == 0) {
          if (ball.velCol > 0) {
            ball.velCol++;
          } else {
            ball.velCol--;
          }
          if (ball.velRow > 0) {
            ball.velRow++;
          } else {
            ball.velRow--;
          }
        }
        if (KEY_DOWN(BUTTON_UP, currentButtons)) {
          if (player.row - 1 < 20) {
            player.row = 20;
          } else {
            player.row -= 1;
          }
        }

        if (KEY_DOWN(BUTTON_DOWN, currentButtons)) {
          if (player.row + player.height + 1 > 160) {
            player.row = HEIGHT - player.height;
          } else {
            player.row += 1;
          }
        }

        if (KEY_DOWN(BUTTON_LEFT, currentButtons)) {
          if (player.col - 1 < 0) {
            player.col = 0;
          } else {
            player.col -= 1;
          }
        }

        if (KEY_DOWN(BUTTON_RIGHT, currentButtons)) {
          if (player.col + player.width + 1 > 120) {
            player.col = 120 - player.width;
          } else {
            player.col += 1;
          }
        }
        ball.row += ball.velRow;
        ball.col += ball.velCol;
        // Check ball bounds
        if (ball.row + ball.velRow < 20) {
          ball.velRow *= -1;
        }
        if (ball.row + ball.velRow + ball.size > 160) {
          ball.velRow *= -1;
        }
        if (ball.col + ball.velCol < 0) {
          enemyScore++;
          fillScreenDMA(BLACK);
          drawScores(playerScore, enemyScore);
          // Reset ball position
          ball.row = 80;
          ball.col = 120;
        }
        // Check if player LOSE
        if (enemyScore > 2) {
            state = LOSE;
            fillScreenDMA(BLACK);
            drawImageDMA(50, 70, LOSE_WIDTH, LOSE_HEIGHT, lose);
            drawString(140, 40, "Press BACKSPACE to Restart", WHITE);
          break;
        }
        if (ball.col + ball.velCol + ball.size > WIDTH) {
          playerScore++;
          fillScreenDMA(BLACK);
          drawScores(playerScore, enemyScore);
          // Reset ball position
          ball.row = 80;
          ball.col = 120;
        }
        // Check if player WIN
        if (playerScore > 2) {
            state = WIN;
            drawFullScreenImageDMA(win);
            drawString(140, 40, "Press BACKSPACE to Restart", WHITE);
          break;
        }

        // Check collision between player - ball and enemy - ball
        if (objCollision(player, ball) || objCollision(enemy, ball)) {
          ball.velCol *= -1;
          fillScreenDMA(BLACK);
          drawScores(playerScore, enemyScore);
        }

        int enemyCenterRow = enemy.row + enemy.height / 2;
        if (ball.row > enemyCenterRow) {
          if (enemy.row + enemy.height + 1 > 160) {
            enemy.row = HEIGHT - enemy.height;
          } else {
            enemy.row += 1;
          }
        }
        if (ball.row < enemyCenterRow) {
          if (enemy.row - 1 < 0) {
            enemy.row = 0;
          } else {
            enemy.row -= 1;
          }
        }

        //////////////////// REDRAWING PLAYER BELOW ////////////////////////
        drawRectDMA(player.row, player.col, player.width, player.height, BLUE);
        drawRectDMA(enemy.row, enemy.col, enemy.width, enemy.height, RED);
        drawRectDMA(ball.row, ball.col, ball.size, ball.size, ORANGE);

        break;
      case WIN:
 
        break;
      case LOSE:

        break;
    }

    previousButtons = currentButtons; // Store the current state of the buttons
  }

  UNUSED(previousButtons); // You can remove this once previousButtons is used

  return 0;
}

int objCollision(struct player player, struct ball ball) {
  return
    player.col < ball.col + ball.size &&
    player.col + player.width > ball.col &&
    player.row < ball.row + ball.size &&
    player.row + player.height > ball.row;
}

void drawScores(int playerScore, int enemyScore) {
  char playerScoreText[20];
  char enemyScoreText[20];
  sprintf(playerScoreText, "Player: %d", playerScore);
  sprintf(enemyScoreText, "Bot: %d", enemyScore);

  drawString(10, 10, playerScoreText, WHITE);
  drawString(10, 130, enemyScoreText, WHITE);
}

void moveText(int *textY) {
    *textY += 1;
}