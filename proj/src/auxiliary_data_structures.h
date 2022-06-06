#ifndef __AUX
#define __AUX

#include <stdint.h>
#include <lcom/xpm.h>
#include "XPMs/cursor.xpm"

enum direction { UP,
                 DOWN,
                 LEFT,
                 RIGHT,
                 UNCHANGED };

enum player { BLUE,
              ORANGE };

struct MovementInfo {
  enum direction dir;
  enum player playerColor;
};

enum screenState {
  MAIN,
  S_GAME,
  M_GAME,
  PAUSE,
  GOONE,
  GOTWO,
  QUIT
};

struct PlayerPosition {
  uint16_t x;
  uint16_t y;
  enum direction currentDirection;
};

struct mousePos {
  uint16_t x;
  uint16_t y;
};

struct images{
  xpm_image_t main, gameOver1, gameOver2, pause, cursor;
};

#endif
