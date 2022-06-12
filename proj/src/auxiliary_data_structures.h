#ifndef __AUX
#define __AUX

#include "XPMs/cursor.xpm"
#include <lcom/xpm.h>
#include <stdint.h>

/** @defgroup Auxiliary_Data_Structures Auxiliary_Data_Structures
 * @{
 *
 * Data structures to help in the game development
 */

/**
 * @brief Enumerated type for specifying the direction of the movement
 */
enum direction { UNCHANGED, /*!< There wasn't a change in direction */
                 UP,        /*!< The player is moving up */
                 DOWN,      /*!< The player is moving down */
                 LEFT,      /*!< The player is moving left */
                 RIGHT,     /*!< The player is moving right */
                 STOP,      /*!< The player returned to the menu */
                 BOOST      /*!< The player used the boost */
};

/**
 * @brief Enumerated type for specifying the player
 */
enum player { ME,      /*!< The player is me (player 1)*/
              OTHER }; /*!< The player is not me (player 2) */

/**
 * @brief Struct to store the information regarding each movement in the game
 *
 */
struct MovementInfo {
  enum direction dir;   /*!< The direction of the movement */
  enum player playerID; /*!< The identity of the player responsible for that movement */
};

/**
 * @brief Enumerated type for specifying the current state of the screen/game
 *
 */
enum screenState {
  MAIN,   /*!< The game is the main menu */
  S_GAME, /*!< The game is running in single computer mode */
  M_GAME, /*!< The game is running in multi computer mode */
  PAUSE,  /*!< The game is paused */
  GOONE,  /*!< The game finished and player 1 won */
  GOTWO,  /*!< The game finished and player 2 won */
  QUIT    /*!< The game will exit */
};

/**
 * @brief Struct to store the player position and direction
 *
 */
struct PlayerPosition {
  uint16_t x;                      /*!< The position of the player in the X axis */
  uint16_t y;                      /*!< The position of the player in the Y axis */
  enum direction currentDirection; /*!< The current direction of the player */
};

/**
 * @brief Struct to store the absolute position of the mouse
 *
 */
struct mousePos {
  int16_t x; /*!< The position of the mouse in the X axis */
  int16_t y; /*!< The position of the mouse in the Y axis */
};

/**
 * @brief Struct to store all the images, used in the game, that are loaded from xpm files
 *
 */
struct images {
  xpm_image_t main, gameOver1, gameOver2, pause, cursor;
};

/**
 * @brief Struct to delimit the area occupied by a button
 *
 * Stores the coordinates of the upper left corner and of the down right one.
 */
struct Button {
  uint16_t up_left_x, up_left_y, down_right_x, down_right_y;
};

/**@}*/
#endif /* __AUX */
