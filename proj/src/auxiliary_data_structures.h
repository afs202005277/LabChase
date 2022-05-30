#ifndef __AUX
#define __AUX

#include <stdint.h>

enum direction { UP,
                 DOWN,
                 LEFT,
                 RIGHT, UNCHANGED };

enum player { BLUE,
              ORANGE };

struct MovementInfo {
  enum direction dir;
  enum player playerColor;
};

enum mouseAction { START,
                   QUIT,
                   PAUSE,
                   RESUME };

struct PlayerPosition {
  uint16_t x;
  uint16_t y;
  enum direction currentDirection;
};
#endif
