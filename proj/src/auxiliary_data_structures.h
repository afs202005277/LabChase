#include <stdint.h>

enum direction { UP,
                 DOWN,
                 LEFT,
                 RIGHT };

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
