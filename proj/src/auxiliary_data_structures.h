#ifndef __AUX
#define __AUX

#include <stdint.h>

enum direction { UP,
                 DOWN,
                 LEFT,
                 RIGHT,
                 UNCHANGED };

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
#endif
