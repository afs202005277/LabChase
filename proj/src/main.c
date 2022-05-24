#include "keyboard.h"
#include "mouse.h"
<<<<<<< HEAD
#include "video_new.h"
#include <stdbool.h>
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

=======
#include "video_gr_gameAPI.h"
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

>>>>>>> 50777d42acfdef665966039d7f82f9e2045b06b0
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
  int ipc_status;
  extern int totalInterrupts;
  message msg;
  extern uint8_t scanCode;
  extern struct MovementInfo nextMove;
  int r;
  unsigned char bit_no_timer, bit_no_keyboard, bit_no_mouse;
  bool first_start = true;


  uint16_t x1, y1; // Player one
  uint32_t color1 = 0xFF1F51FF;
  uint16_t x2, y2; // Player two
  uint32_t color2 = 0xFFFF5F1F;
  uint16_t OBJ_SIZE = 10;

  /* Mouse Variables */
  struct packet pp;
  extern uint8_t byteFromMouse;
  uint8_t counter = 0;
  enum mouseAction gameState = PAUSE;

  timer_subscribe_int(&bit_no_timer);
  keyboard_subscribe_int(&bit_no_keyboard);
  mouse_enable_data_reporting();
  mouse_subscribe_int(&bit_no_mouse);
<<<<<<< HEAD
  while (gameState != QUIT) {
=======
  start_game(0x14C);
  while (scanCode != ESC_BREAK_CODE) {
>>>>>>> 50777d42acfdef665966039d7f82f9e2045b06b0
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
              passive_move_players();
            }
          }
          if (msg.m_notify.interrupts & BIT(bit_no_keyboard)) {
            kbc_ih();
            if (nextMove.dir != UNCHANGED)
              move_player(nextMove, false);
          }
          if (msg.m_notify.interrupts & BIT(bit_no_mouse)) {
            mouse_ih();
            if (!(counter == 0 && (byteFromMouse & BIT(3)) == 0)) {
              pp.bytes[counter] = byteFromMouse;
              counter++;
            }
            if (counter == 3) {
              counter = 0;
              parse_mouse_info(&pp, &gameState);
            }
          }
          break;
        default:
          break;
      }
    }

    if (gameState == START) {
      if (first_start) {
        first_start = false;
        new_vg_init(0x14c);
        x1 = get_h_res()/4;
        y1 = get_v_res()/2;
        x2 = get_h_res()/4*3;
        y2 = get_v_res()/2;
      }
      printf("Started?");
      start_screen(x1, y1, color1, x2, y2, color2, OBJ_SIZE);
      gameState = RESUME;
    }
  }
  vg_exit();
  keyboard_unsubscribe_int();
  timer_unsubscribe_int();
  mouse_unsubscribe_int();
  return 0;
}
