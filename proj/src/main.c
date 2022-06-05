#include "keyboard.h"
#include "mouse.h"
#include "rtc.h"
#include "serial.h"
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

int wait_for_connection(uint8_t bit_no_serial) {
  int r, ipc_status;
  message msg;
  extern uint8_t receivedChar;
  send_character('p');
  unsigned char character;
  read_character(&character);
  if (character == 'c')
    return 0;
  while (true) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & BIT(bit_no_serial)) {
            serial_ih();
            if (receivedChar == 'c') {
              send_character('p');
              return 0;
            }
          }
          break;
        default:
          break;
      }
    }
  }
  return 1;
}

int(proj_main_loop)() {
  uint8_t hour;
  extern uint8_t receivedChar;
  int ipc_status;
  extern int totalInterrupts;
  message msg;
  extern uint8_t scanCode;
  extern struct MovementInfo nextMove;
  int r;
  unsigned char bit_no_timer, bit_no_keyboard, bit_no_mouse, bit_no_rtc, bit_no_serial;

  /* Mouse Variables */
  // struct packet pp;
  // extern uint8_t byteFromMouse;
  // uint8_t counter = 0;

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

  while (screenState != QUIT) {

    while (screenState == MAIN) {
      if (!printed) {
        xpm_drawer(MainScreen);
        printed = true;
        startGame = false;
      }
    }

    while (screenState == GOONE) {
      if (!printed) {
        xpm_drawer(GOPOneWins);
        printed = true;
        startGame = false;
      }
    }

    while (screenState == GOTWO) {
      if (!printed) {
        xpm_drawer(GOPTwoWins);
        printed = true;
        startGame = false;
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
        start_game(0x115, hour);
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
    bool connectionSet = false;
    while (screenState == M_GAME) {
      if (!connectionSet) {
        wait_for_connection(bit_no_serial);
        connectionSet = true;
      }
      if (!startGame) { // Function that runs once when game starts
        start_game(0x115, hour);
        startGame = true;
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
              if (totalInterrupts % 20 == 0) {
                int tmp = passive_move_players();
                if (tmp == 1) {
                  screenState = GOTWO;
                }
                else if (tmp == 2) {
                  screenState = GOONE;
                }
              }
            }
            if (msg.m_notify.interrupts & BIT(bit_no_serial)) {
              serial_ih();
              struct MovementInfo mov;
              mov.dir = receivedChar;
              mov.playerID = OTHER;
              bool validReceive = receivedChar == UP || receivedChar == DOWN || receivedChar == LEFT || receivedChar == RIGHT;
              if (validReceive && move_player(mov, false) == 1) {
                screenState = QUIT;
              }
            }
            if (msg.m_notify.interrupts & BIT(bit_no_keyboard)) {
              kbc_ih();
              if (nextMove.dir != UNCHANGED) {
                send_character(nextMove.dir);
                if (nextMove.playerID == ME && move_player(nextMove, false) == 1) {
                  screenState = QUIT;
                }
              }
            }
            /* if (msg.m_notify.interrupts & BIT(bit_no_mouse)) {
              mouse_ih();
              if (!(counter == 0 && (byteFromMouse & BIT(3)) == 0)) {
                pp.bytes[counter] = byteFromMouse;
                counter++;
              }
              if (counter == 3) {
                counter = 0;
                parse_mouse_info(&pp, &gameState);
              }
            } */
            break;
          default:
            break;
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
