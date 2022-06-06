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

enum screenState screenState = MAIN;

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

int(proj_main_loop)() {
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

  bool printed = false;
  bool startGame = false;
  bool paused = false;

  void *saveGameScreen = malloc(get_h_res() * get_v_res() * get_bits_per_pixel() / 8);

  setMouseInitPos();

  while (screenState != QUIT) {

    while (screenState == MAIN) {
      if (!printed) {
        xpm_drawer(MainScreen);
        printed = true;
        startGame = false;
      }
      if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
        printf("driver_receive failed with: %d", r);
        continue;
      }
      if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE:                                       /* hardware interrupt notification */
            if (msg.m_notify.interrupts & BIT(bit_no_mouse)) { /* subscribed interrupt */
              mouse_ih();
              if (!(counter == 0 && (byteFromMouse & BIT(3)) == 0)) {
                pp.bytes[counter] = byteFromMouse;
                counter++;
              }
              if (counter == 3) {
                counter = 0;
                parse_mouse_bytes(&pp);
                mouse_print_packet(&pp);
                xpm_drawer(MainScreen);
                mouseMovement(pp.delta_x, pp.delta_y);
                if (mouseInPlace(253, 288, 547, 313) && pp.lb) {
                  screenState = S_GAME;
                }
                else if (mouseInPlace(268, 352, 532, 376) && pp.lb) {
                  screenState = M_GAME;
                }
                else if (mouseInPlace(358, 412, 442, 437) && pp.lb) {
                  screenState = QUIT;
                }
              }
            }
        }
      }
    }

    while (screenState == GOONE) {
      if (!printed) {
        xpm_drawer(GOPOneWins);
        printed = true;
        startGame = false;
      }
            if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
        printf("driver_receive failed with: %d", r);
        continue;
      }
      if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE:                                       /* hardware interrupt notification */
            if (msg.m_notify.interrupts & BIT(bit_no_mouse)) { /* subscribed interrupt */
              mouse_ih();
              if (!(counter == 0 && (byteFromMouse & BIT(3)) == 0)) {
                pp.bytes[counter] = byteFromMouse;
                counter++;
              }
              if (counter == 3) {
                counter = 0;
                parse_mouse_bytes(&pp);
                mouse_print_packet(&pp);
                xpm_drawer(GOPOneWins);
                mouseMovement(pp.delta_x, pp.delta_y);
                if (mouseInPlace(253, 288, 547, 313) && pp.lb) {
                  screenState = S_GAME;
                }
                else if (mouseInPlace(268, 352, 532, 376) && pp.lb) {
                  screenState = M_GAME;
                }
                else if (mouseInPlace(358, 412, 442, 437) && pp.lb) {
                  screenState = QUIT;
                }
              }
            }
        }
      }
    }

    while (screenState == GOTWO) {
      if (!printed) {
        xpm_drawer(GOPTwoWins);
        printed = true;
        startGame = false;
      }
            if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
        printf("driver_receive failed with: %d", r);
        continue;
      }
      if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE:                                       /* hardware interrupt notification */
            if (msg.m_notify.interrupts & BIT(bit_no_mouse)) { /* subscribed interrupt */
              mouse_ih();
              if (!(counter == 0 && (byteFromMouse & BIT(3)) == 0)) {
                pp.bytes[counter] = byteFromMouse;
                counter++;
              }
              if (counter == 3) {
                counter = 0;
                parse_mouse_bytes(&pp);
                mouse_print_packet(&pp);
                xpm_drawer(GOPTwoWins);
                mouseMovement(pp.delta_x, pp.delta_y);
                if (mouseInPlace(253, 288, 547, 313) && pp.lb) {
                  screenState = S_GAME;
                }
                else if (mouseInPlace(268, 352, 532, 376) && pp.lb) {
                  screenState = M_GAME;
                }
                else if (mouseInPlace(358, 412, 442, 437) && pp.lb) {
                  screenState = QUIT;
                }
              }
            }
        }
      }
    }

    while (screenState == PAUSE) {
      if (!printed) {
        memcpy(saveGameScreen, get_video_mem(), get_h_res() * get_v_res() * get_bits_per_pixel() / 8);
        printed = true;
        paused = true;
        xpm_drawer(PauseScreen);
      }
      if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
        printf("driver_receive failed with: %d", r);
        continue;
      }
      if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE:
            if (msg.m_notify.interrupts & BIT(bit_no_keyboard)) {
              kbc_ih();
              if (nextMove.dir != UNCHANGED)
                if (move_player(nextMove, false) == 1) {
                  screenState = QUIT;
                }
            }
            break;
          default:
            break;
        }
      }
    }

    while (screenState == S_GAME) {
      if (!startGame) {
        memset((char *)get_video_mem(), 0, get_h_res() * get_v_res() * (get_bits_per_pixel() + 7)/8);
        start_game(hour);
        startGame = true;
        printed = false;
      }
      if (paused) {
        memcpy(get_video_mem(), saveGameScreen, get_h_res() * get_v_res() * get_bits_per_pixel() / 8);
        paused = false;
        printed = false;
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
              if (totalInterrupts % 5 == 0) {
                int tmp = passive_move_players();
                if (tmp == 1) {
                  screenState = GOTWO;
                }
                else if (tmp == 2) {
                  screenState = GOONE;
                }
              }
            }
            if (msg.m_notify.interrupts & BIT(bit_no_keyboard)) {
              kbc_ih();
              if (nextMove.dir != UNCHANGED)
                if (move_player(nextMove, false) == 1) {
                  screenState = QUIT;
                }
            }
            break;
          default:
            break;
        }
      }
    }
    while (screenState == M_GAME) {
      if (!startGame) { // Function that runs once when game starts
        memset((char *)get_video_mem(), 0, get_h_res() * get_v_res() * (get_bits_per_pixel() + 7)/8);
        start_game(hour);
        startGame = true;
        printed = false;
      }
      // Multiplayer
    }
  }
  vg_exit();
  keyboard_unsubscribe_int();
  timer_unsubscribe_int();
  mouse_unsubscribe_int();
  rtc_unsubscribe_int();
  return 0;
}
