#include "keyboard.h"
#include "mouse.h"
#include "serial.h"
#include "video_gr_gameAPI.h"
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

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
            if (receivedChar == 'c'){
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
  extern uint8_t receivedChar;
  int ipc_status;
  extern int totalInterrupts;
  message msg;
  extern uint8_t scanCode;
  extern struct MovementInfo nextMove;
  int r;
  unsigned char bit_no_timer, bit_no_keyboard, bit_no_mouse, bit_no_serial;

  /* Mouse Variables */
  struct packet pp;
  extern uint8_t byteFromMouse;
  uint8_t counter = 0;
  enum mouseAction gameState = PAUSE;

  timer_subscribe_int(&bit_no_timer);
  keyboard_subscribe_int(&bit_no_keyboard);
  mouse_enable_data_reporting();
  mouse_subscribe_int(&bit_no_mouse);
  serial_subscribe(&bit_no_serial);

  wait_for_connection(bit_no_serial);
  start_game(0x14C);
  bool continueLoop = true;
  while (gameState != QUIT && continueLoop && scanCode != ESC_BREAK_CODE) {
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
              if (passive_move_players() == 1) {
                continueLoop = false;
              }
            }
          }
          if (msg.m_notify.interrupts & BIT(bit_no_serial)) {
            serial_ih();
            struct MovementInfo mov;
            mov.dir = receivedChar;
            mov.playerColor = OTHER;
            bool validReceive = receivedChar == UP || receivedChar == DOWN || receivedChar == LEFT || receivedChar == RIGHT;
            if (validReceive && move_player(mov, false) == 1) {
              continueLoop = false;
            }
          }
          if (msg.m_notify.interrupts & BIT(bit_no_keyboard)) {
            kbc_ih();
            if (nextMove.dir != UNCHANGED) {
              send_character(nextMove.dir);
              if (nextMove.playerColor == ME && move_player(nextMove, false) == 1) {
                continueLoop = false;
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
              parse_mouse_info(&pp, &gameState);
            }
          }
          break;
        default:
          break;
      }
    }
  }
  vg_exit();
  keyboard_unsubscribe_int();
  timer_unsubscribe_int();
  mouse_unsubscribe_int();
  return 0;
}
