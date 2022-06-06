#include "keyboard.h"
#include "mouse.h"
#include "rtc.h"
#include "video_gr_gameAPI.h"
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

#include "XPMs/GameOverPlayerOneWins.xpm"
#include "XPMs/GameOverPlayerTwoWins.xpm"
#include "XPMs/MainScreen.xpm"
#include "XPMs/PauseScreen.xpm"

enum screenState screenState = S_GAME;

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
  struct images imgs;
  load_images(&imgs);
  uint8_t hour;
  int ipc_status;
  extern int totalInterrupts;
  message msg;
  extern uint8_t scanCode;
  extern struct MovementInfo nextMove;
  int r;
  unsigned char bit_no_timer, bit_no_keyboard, bit_no_mouse, bit_no_rtc;

  /* Mouse Variables */
  struct packet pp;
  extern uint8_t byteFromMouse;
  uint8_t counter = 0;

  timer_subscribe_int(&bit_no_timer);
  keyboard_subscribe_int(&bit_no_keyboard);
  mouse_enable_data_reporting();
  mouse_subscribe_int(&bit_no_mouse);
  rtc_subscribe_int(&bit_no_rtc);

  read_hours(&hour);

  if (new_vg_init(0x115) != 0)
    return 1;

  bool startGame = false;
  bool paused = false;

  void *saveGameScreen = malloc(get_h_res() * get_v_res() * get_bits_per_pixel() / 8);

  draw_img(imgs.main, 0, 0);
  setMouseInitPos(imgs.cursor);

  while (screenState != QUIT) {
    if (!startGame && screenState == S_GAME) {
      start_game(hour);
      startGame = true;
    }
    if (paused && screenState == S_GAME) {
      memcpy(get_video_mem(), saveGameScreen, get_h_res() * get_v_res() * get_bits_per_pixel() / 8);
      paused = false;
    }
    if (!paused && screenState == PAUSE) {
      memcpy(saveGameScreen, get_video_mem(), get_h_res() * get_v_res() * get_bits_per_pixel() / 8);
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
            if (screenState == S_GAME || screenState == M_GAME) {
              if (totalInterrupts % 5 == 0) {
                int tmp = passive_move_players();
                if (tmp == 1) {
                  screenState = GOTWO;
                  draw_img(imgs.gameOver2, 0, 0);
                  startGame = false;
                  nextMove.dir = UNCHANGED;
                }
                else if (tmp == 2) {
                  screenState = GOONE;
                  draw_img(imgs.gameOver1, 0, 0);
                  startGame = false;
                  nextMove.dir = UNCHANGED;
                }
              }
            }
          }
          if (msg.m_notify.interrupts & BIT(bit_no_keyboard)) {
            kbc_ih();
            if (screenState == S_GAME || screenState == M_GAME) {
              if (nextMove.dir != UNCHANGED)
                if (move_player(nextMove, false) == 1) {
                  screenState = QUIT;
                }
            }
          }
          if (msg.m_notify.interrupts & BIT(bit_no_mouse)) {
            mouse_ih();
            if (!(counter == 0 && (byteFromMouse & BIT(3)) == 0)) {
              pp.bytes[counter] = byteFromMouse;
              counter++;
            }
            if (counter == 3) {
              counter = 0;
              parse_mouse_bytes(&pp);
            }
            if (screenState == MAIN || screenState == GOONE || screenState == GOTWO) {
              switch (screenState) {
                case MAIN:
                  draw_img(imgs.main, 0, 0);
                  mouseMovement(pp.delta_x, pp.delta_y, imgs.cursor);
                  if (mouseInPlace(253, 288, 547, 313) && pp.lb) {
                    screenState = S_GAME;
                  }
                  else if (mouseInPlace(268, 352, 532, 376) && pp.lb) {
                    screenState = M_GAME;
                  }
                  else if (mouseInPlace(358, 412, 442, 437) && pp.lb) {
                    screenState = QUIT;
                  }
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
                  break;
                default:
                  break;
              }
            }
          }
      }
    }
  }
  vg_exit();
  keyboard_unsubscribe_int();
  timer_unsubscribe_int();
  mouse_unsubscribe_int();
  rtc_unsubscribe_int();
  return 0;
}
