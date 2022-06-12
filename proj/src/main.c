#include "timer_proj.h"
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

#define GRAPHICS_MODE 0x115
#define SEND_BYTE 'p'
#define RECEIVE_BYTE 'c'
#define BOOST_INCREMENT 4

uint8_t boostPlayer1;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/src/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/src/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

void load_images(struct images *imgs) {
  imgs->main = load_image(MainScreen);
  imgs->pause = load_image(PauseScreen);
  imgs->gameOver1 = load_image(GOPOneWins);
  imgs->gameOver2 = load_image(GOPTwoWins);
  imgs->cursor = load_image(Cursor);
}

int(proj_main_loop)() {
  enum screenState screenState = MAIN;

  struct MovementInfo nextMove;
  memset(&nextMove, 0, sizeof(nextMove));

  struct images imgs;
  load_images(&imgs);

  struct packet pp;
  memset(&pp, 0, sizeof(pp));

  message msg;

  uint8_t bit_no_timer, bit_no_keyboard, bit_no_mouse, bit_no_rtc, bit_no_serial;
  uint8_t mainSpeed = 5, hour, numBytesReceivedMouse = 0, mouseReceivedByte;
  uint8_t speedOffsetPlayer1 = 0, speedOffsetPlayer2 = 0;

  extern uint8_t receivedChar;
  int r, ipc_status;
  timer_subscribe_int(&bit_no_timer);
  keyboard_subscribe_int(&bit_no_keyboard);
  mouse_enable_data_reporting();
  mouse_subscribe_int(&bit_no_mouse);
  rtc_subscribe_int(&bit_no_rtc);
  serial_subscribe(&bit_no_serial);

  if (new_vg_init(GRAPHICS_MODE) == NULL)
    return 1;
  bool startGame = false, paused = false, isWaiting = false, isConnected = false;

  unsigned long numBytesSavedGame = (get_h_res() * get_v_res() * get_bits_per_pixel() + 7) / 8;
  void *saveGameScreen = malloc(numBytesSavedGame);

  draw_img(imgs.main, 0, 0);
  setMouseInitPos(imgs.cursor);

  while (screenState != QUIT) {
    if (!startGame && (screenState == S_GAME || screenState == M_GAME)) {
      read_hours(&hour);
      start_game(hour);
      startGame = true;
    }
    if (paused && screenState == S_GAME) {
      memcpy(get_video_mem(), saveGameScreen, numBytesSavedGame);
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
            if (!isWaiting) {
              if (screenState == S_GAME || screenState == M_GAME) {
                if (get_num_interrupts_timer() % (mainSpeed - speedOffsetPlayer1) == 0) {
                  if (get_num_interrupts_timer() % 2 == 0) {
                    if (((int) speedOffsetPlayer1) - 1 < 0)
                      speedOffsetPlayer1 = 0;
                    else
                      speedOffsetPlayer1 -= 1;
                  }
                  struct MovementInfo mov;
                  mov.dir = UNCHANGED;
                  mov.playerID = ME;
                  if (move_player(mov, true, speedOffsetPlayer1 > 0) != 0) {
                    screenState = GOTWO;
                    draw_img(imgs.gameOver2, 0, 0);
                    startGame = false;
                    isConnected = false;
                  }
                }
                if (get_num_interrupts_timer() % (mainSpeed - speedOffsetPlayer2) == 0) {
                  if (get_num_interrupts_timer() % 2 == 0) {
                    if (((int) speedOffsetPlayer2) - 1 < 0)
                      speedOffsetPlayer2 = 0;
                    else
                      speedOffsetPlayer2 -= 1;
                  }
                  struct MovementInfo mov;
                  mov.dir = UNCHANGED;
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
          }
          if (msg.m_notify.interrupts & BIT(bit_no_serial)) {
            serial_ih();
            if (receivedChar == STOP) {
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
              mainSpeed = 20;
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
            if (!isWaiting) {
              if (screenState == S_GAME || screenState == M_GAME) {
                if (nextMove.dir == BOOST) {
                  if (nextMove.playerID == ME && speedOffsetPlayer1 == 0) {
                    speedOffsetPlayer1 += BOOST_INCREMENT;
                  }
                  else if (nextMove.playerID == OTHER && speedOffsetPlayer2 == 0) {
                    speedOffsetPlayer2 += BOOST_INCREMENT;
                  }
                }
                else if (nextMove.dir == STOP) {
                  isConnected = false;
                  startGame = false;
                  screenState = MAIN;
                  draw_img(imgs.main, 0, 0);
                }
                else if (nextMove.dir != UNCHANGED) {
                  bool isBoosted = (nextMove.playerID == ME && speedOffsetPlayer1 > 0) || (nextMove.playerID == OTHER && speedOffsetPlayer2 > 0);
                  if (isConnected) {
                    send_character(nextMove.dir);
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
              if (!isWaiting) {
                if (screenState == MAIN || screenState == GOONE || screenState == GOTWO) {
                  switch (screenState) {
                    case MAIN:
                      draw_img(imgs.main, 0, 0);
                      mouseMovement(pp.delta_x, pp.delta_y, imgs.cursor);
                      if (mouseInPlace(253, 288, 547, 313) && pp.lb) {
                        screenState = S_GAME;
                        pp.lb = false;
                      }
                      else if (mouseInPlace(268, 352, 532, 376) && pp.lb) {
                        isWaiting = true;
                        send_character(SEND_BYTE);
                        unsigned char character;
                        read_character(&character);
                        if (character == RECEIVE_BYTE) {
                          screenState = M_GAME;
                          isConnected = true;
                          isWaiting = false;
                        }
                        pp.lb = false;
                      }
                      else if (mouseInPlace(358, 412, 442, 437) && pp.lb) {
                        screenState = QUIT;
                        pp.lb = false;
                      }
                      memset(&pp, 0, sizeof(pp));
                      break;
                    case GOONE:
                      draw_img(imgs.gameOver1, 0, 0);
                      mouseMovement(pp.delta_x, pp.delta_y, imgs.cursor);
                      if (mouseInPlace(236, 341, 367, 371) && pp.lb) {
                        screenState = S_GAME;
                        pp.lb = false;
                      }
                      else if (mouseInPlace(451, 341, 565, 372) && pp.lb) {
                        screenState = MAIN;
                        startGame = false;
                        pp.lb = false;
                      }
                      break;
                      memset(&pp, 0, sizeof(pp));
                    case GOTWO:
                      draw_img(imgs.gameOver2, 0, 0);
                      mouseMovement(pp.delta_x, pp.delta_y, imgs.cursor);
                      if (mouseInPlace(236, 341, 367, 371) && pp.lb) {
                        screenState = S_GAME;
                        pp.lb = false;
                      }
                      else if (mouseInPlace(451, 341, 565, 372) && pp.lb) {
                        screenState = MAIN;
                        startGame = false;
                        pp.lb = false;
                      }
                      memset(&pp, 0, sizeof(pp));
                      break;
                    default:
                      break;
                  }
                }
              }
            }
          }
          break;
      }
    }
  }
  serial_unsubscribe();
  timer_unsubscribe_int();
  keyboard_unsubscribe_int();
  mouse_unsubscribe_int();
  disable_data_reporting();
  rtc_unsubscribe_int();
  vg_exit();
  return 0;
}
