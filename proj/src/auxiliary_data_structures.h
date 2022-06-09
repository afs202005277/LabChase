#ifndef __AUX
#define __AUX

#include "XPMs/cursor.xpm"
#include <lcom/xpm.h>
#include <stdint.h>

enum direction { UNCHANGED,
                 UP,
                 DOWN,
                 LEFT,
                 RIGHT };

enum player { ME,
              OTHER };

struct MovementInfo {
  enum direction dir;
  enum player playerID;
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

struct images {
  xpm_image_t main, gameOver1, gameOver2, pause, cursor;
};

#endif
