#include "adapted_timer.h"
#include "buttons.c"
#include "keyboard.h"
#include "mouse.h"
#include "rtc.h"
#include "serial.h"
#include "video_gr_gameAPI.h"
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

#include "XPMs/GameOverPlayerOneWins.h"
#include "XPMs/GameOverPlayerTwoWins.h"
#include "XPMs/MainScreen.h"
#include "XPMs/PauseScreen.h"

/** @defgroup Main Main
 * @{
 *
 * The driver code of the game
 */

#define GRAPHICS_MODE 0x115 /*!< The graphics mode in which the game will operate */
#define SEND_BYTE 'p'       /*!< The byte this device is supposed to send (to estabilish a connection with the other end of the serial port) */
#define RECEIVE_BYTE 'c'    /*!< The byte this device is supposed to receive (to estabilish a connection with the other end of the serial port) */
#define BOOST_INCREMENT 4   /*!< The speed increment of boosting */
#define S_SPEED 5           /*!< Speed of the game, when the game is running in just one device */
#define M_SPEED 20          /*!<  Speed of the game, when the game is running in two devices */

static uint8_t bit_no_timer, bit_no_keyboard, bit_no_mouse, bit_no_serial;

/**
 * @brief Subscribes interrupts from all the devices and enables data reporting (mouse)
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int set_up_devices() {
  if (timer_subscribe_int(&bit_no_timer) != OK) {
    printf("Failed to subscribe timer interrupts!\n");
    return 1;
  }
  if (keyboard_subscribe_int(&bit_no_keyboard) != OK) {
    printf("Failed to subscribe keyboard interrupts!\n");
    return 2;
  }
  if (mouse_enable_data_reporting() != OK) {
    printf("Failed to enable data reporting!\n");
    return 3;
  }
  if (mouse_subscribe_int(&bit_no_mouse) != OK) {
    printf("Failed to subscribe mouse interrupts!\n");
    return 4;
  }
  if (serial_subscribe(&bit_no_serial) != OK) {
    printf("Failed to subscribe serial port interrupts!\n");
    return 6;
  }
  return 0;
}

/**
 * @brief Unsubscribes all the interrupts and resets the mouse to Minix's default configuration
 *
 */
void exit_devices() {
  serial_unsubscribe();
  timer_unsubscribe_int();
  keyboard_unsubscribe_int();
  mouse_unsubscribe_int();
  disable_data_reporting();
  vg_exit();
}

/**
 * @brief Loads all xpm images into the struct provided
 *
 * @param imgs memory address of the struct to be filled with the loaded images
 */
void load_images(struct images *imgs) {
  imgs->main = load_image(MainScreen);
  imgs->pause = load_image(PauseScreen);
  imgs->gameOver1 = load_image(GOPOneWins);
  imgs->gameOver2 = load_image(GOPTwoWins);
  imgs->cursor = load_image(Cursor);
}

uint8_t convert_from_bcd(uint8_t bcdNum) {
  uint8_t decNum = (bcdNum >> 4);
  decNum *= 10;
  decNum += (bcdNum & 0x0F);
  return decNum;
}

/**
 * @brief Starts the game and changes background according to the current hour
 *
 */
void start_game() {
  uint8_t hour;
  read_hours(&hour);
  uint8_t decHour = convert_from_bcd(hour);
  set_up_game(decHour);
}

/**
 * @brief updates the speed offset of a given player
 *
 * @param speed memory address of the speed offset of the player
 */
void update_speed(uint8_t *speed) {
  if (get_num_interrupts_timer() % 2 == 0) {
    if (((int) *speed) - 1 < 0)
      *speed = 0;
    else
      *speed -= 1;
  }
}

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/g02/proj/src/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g02/proj/src/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

/**
 * @brief Handles the game logic and assign tasks to the different devices
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(proj_main_loop)() {
  enum screenState screenState = MAIN;

  struct MovementInfo nextMove;
  memset(&nextMove, 0, sizeof(nextMove));

  struct images imgs;
  load_images(&imgs);

  struct packet pp;
  memset(&pp, 0, sizeof(pp));

  message msg;

  uint8_t mainSpeed = S_SPEED, numBytesReceivedMouse = 0, mouseReceivedByte;
  uint8_t speedOffsetPlayer1 = 0, speedOffsetPlayer2 = 0;

  extern uint8_t receivedChar;
  int r, ipc_status;

  if (new_vg_init(GRAPHICS_MODE) == NULL) {
    printf("Couldn't init screen!\n");
    return 1;
  }
  bool startGame = false, paused = false, isWaiting = false, isConnected = false;

  unsigned long numBytesSavedGame = (get_h_res() * get_v_res() * get_bits_per_pixel() + 7) / 8;
  void *saveGameScreen = malloc(numBytesSavedGame);

  draw_img(imgs.main, 0, 0);
  setMouseInitPos(imgs.cursor);
  if (set_up_devices() != OK)
    return 1;
  while (screenState != QUIT) {
    if (!startGame && (screenState == S_GAME || screenState == M_GAME)) {
      start_game();
      startGame = true;
    }
    if (paused && screenState == S_GAME) {
      draw_saved_game(saveGameScreen, numBytesSavedGame);
      paused = false;
    }
    if (!paused && screenState == PAUSE) {
      memcpy(saveGameScreen, get_video_mem(), numBytesSavedGame);
      paused = true;
      draw_img(imgs.pause, 0, 0);
    }
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & BIT(bit_no_timer)) {
            timer_int_handler();
            if (!isWaiting && (screenState == S_GAME || screenState == M_GAME)) {
              struct MovementInfo mov;
              mov.dir = UNCHANGED;
              if (get_num_interrupts_timer() % (mainSpeed - speedOffsetPlayer1) == 0) {
                update_speed(&speedOffsetPlayer1);
                mov.playerID = ME;
                if (move_player(mov, true, speedOffsetPlayer1 > 0) != 0) {
                  screenState = GOTWO;
                  draw_img(imgs.gameOver2, 0, 0);
                  startGame = false;
                  isConnected = false;
                }
              }
              if (get_num_interrupts_timer() % (mainSpeed - speedOffsetPlayer2) == 0) {
                update_speed(&speedOffsetPlayer2);
                mov.playerID = OTHER;
                if (move_player(mov, true, speedOffsetPlayer2 > 0) != 0) {
                  screenState = GOONE;
                  draw_img(imgs.gameOver1, 0, 0);
                  startGame = false;
                  isConnected = false;
                }
              }
            }
          }
          if (msg.m_notify.interrupts & BIT(bit_no_serial)) {
            serial_ih();
            uint8_t playerID = receivedChar & BIT(7);
            receivedChar &= 0x7F;
            if (receivedChar == BOOST) {
              if (playerID == OTHER && speedOffsetPlayer1 == 0) {
                speedOffsetPlayer1 += BOOST_INCREMENT;
              }
              else if (playerID == ME && speedOffsetPlayer2 == 0) {
                speedOffsetPlayer2 += BOOST_INCREMENT;
              }
            }
            else if (receivedChar == STOP) {
              isConnected = false;
              startGame = false;
              screenState = MAIN;
              draw_img(imgs.main, 0, 0);
            }
            if (isWaiting && receivedChar == RECEIVE_BYTE) {
              send_character(SEND_BYTE);
              isConnected = true;
              isWaiting = false;
              screenState = M_GAME;
              mainSpeed = M_SPEED;
            }
            else if (isConnected) {
              struct MovementInfo mov;
              mov.dir = receivedChar;
              mov.playerID = OTHER;
              bool validReceive = receivedChar == UP || receivedChar == DOWN || receivedChar == LEFT || receivedChar == RIGHT;
              if (validReceive && move_player(mov, false, false) == 1) {
                screenState = QUIT;
              }
            }
          }
          if (msg.m_notify.interrupts & BIT(bit_no_keyboard)) {
            nextMove = key_code_interpreter(&screenState);
            if (!isWaiting && (screenState == S_GAME || screenState == M_GAME)) {
              if (nextMove.dir == BOOST) {
                uint8_t tmp = BOOST | (nextMove.playerID << 7);
                send_character(tmp);
                if (nextMove.playerID == ME && speedOffsetPlayer1 == 0) {
                  speedOffsetPlayer1 += BOOST_INCREMENT;
                }
                else if (nextMove.playerID == OTHER && speedOffsetPlayer2 == 0) {
                  speedOffsetPlayer2 += BOOST_INCREMENT;
                }
              }
              else if (nextMove.dir == STOP) {
                send_character(STOP);
                isConnected = false;
                startGame = false;
                screenState = MAIN;
                draw_img(imgs.main, 0, 0);
              }
              else if (nextMove.dir != UNCHANGED) {
                bool isBoosted = (nextMove.playerID == ME && speedOffsetPlayer1 > 0) || (nextMove.playerID == OTHER && speedOffsetPlayer2 > 0);
                if (isConnected) {
                  uint8_t id = nextMove.playerID;
                  id <<= 7;
                  send_character(nextMove.dir | id);
                  if (nextMove.playerID == ME && move_player(nextMove, false, isBoosted) == 1) {
                    isConnected = false;
                    screenState = GOONE;
                    draw_img(imgs.gameOver1, 0, 0);
                    startGame = false;
                  }
                }
                else {
                  if (move_player(nextMove, false, isBoosted) == 1) {
                    screenState = GOONE;
                    draw_img(imgs.gameOver1, 0, 0);
                    startGame = false;
                  }
                }
              }
            }
          }
          if (msg.m_notify.interrupts & BIT(bit_no_mouse)) {
            mouse_ih();
            mouseReceivedByte = get_received_byte();
            if (!(numBytesReceivedMouse == 0 && (mouseReceivedByte & BIT(3)) == 0)) {
              pp.bytes[numBytesReceivedMouse] = mouseReceivedByte;
              numBytesReceivedMouse++;
            }
            if (numBytesReceivedMouse == PACKET_SIZE) {
              numBytesReceivedMouse = 0;
              parse_mouse_bytes(&pp);
              if (!isWaiting && (screenState == MAIN || screenState == GOONE || screenState == GOTWO)) {
                switch (screenState) {
                  case MAIN:
                    draw_img(imgs.main, 0, 0);
                    mouseMovement(pp.delta_x, pp.delta_y, imgs.cursor);
                    if (mouseInPlace(single_main) && pp.lb) {
                      screenState = S_GAME;
                      mainSpeed = S_SPEED;
                    }
                    else if (mouseInPlace(multi_main) && pp.lb) {
                      isWaiting = true;
                      send_character(SEND_BYTE);
                      unsigned char character;
                      read_character(&character);
                      if (character == RECEIVE_BYTE) {
                        screenState = M_GAME;
                        isConnected = true;
                        isWaiting = false;
                      }
                    }
                    else if (mouseInPlace(quit_main) && pp.lb) {
                      screenState = QUIT;
                    }
                    memset(&pp, 0, sizeof(pp));
                    break;
                  case GOONE:
                    draw_img(imgs.gameOver1, 0, 0);
                    mouseMovement(pp.delta_x, pp.delta_y, imgs.cursor);
                    if (mouseInPlace(play_GO) && pp.lb) {
                      screenState = S_GAME;
                      mainSpeed = S_SPEED;
                    }
                    else if (mouseInPlace(main_GO) && pp.lb) {
                      screenState = MAIN;
                      startGame = false;
                    }
                    break;
                    memset(&pp, 0, sizeof(pp));
                  case GOTWO:
                    draw_img(imgs.gameOver2, 0, 0);
                    mouseMovement(pp.delta_x, pp.delta_y, imgs.cursor);
                    if (mouseInPlace(play_GO) && pp.lb) {
                      screenState = S_GAME;
                      mainSpeed = S_SPEED;
                    }
                    else if (mouseInPlace(main_GO) && pp.lb) {
                      screenState = MAIN;
                      startGame = false;
                    }
                    memset(&pp, 0, sizeof(pp));
                    break;
                  default:
                    break;
                }
              }
            }
          }
          break;
      }
    }
  }
  exit_devices();
  return 0;
}
