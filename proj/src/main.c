#include <lcom/lcf.h>
// lcom_stop proj
#include "keyboard.h"
#include "mouse.h"
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

enum mouseAction { START,
                   QUIT,
                   PAUSE,
                   RESUME };

int main(int argc, char *argv[]) {
  int ipc_status;
  extern int totalInterrupts;
  message msg;
  int r;
  unsigned char bit_no_timer, bit_no_keyboard, bit_no_mouse;
  timer_subscribe_int(&bit_no_timer);
  keyboard_subscribe_int(&bit_no_keyboard);
  mouse_enable_data_reporting();
  mouse_subscribe_int(&bit_no_mouse);
  while (true) { /* You may want to use a different condition */
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                       /* hardware interrupt notification */
          if (msg.m_notify.interrupts & BIT(bit_no_timer)) { /* subscribed interrupt */
            timer_int_handler();                             /* process it */
          }
          if (msg.m_notify.interrupts & BIT(bit_no_keyboard)) {
            kbc_ih();
          }
          if (msg.m_notify.interrupts & BIT(bit_no_mouse)) {
            mouse_ih();
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else { /* received a standard message, not a notification */
           /* no standard messages expected: do nothing */
    }
  }
  keyboard_unsubscribe_int();
  timer_unsubscribe_int();
  mouse_unsubscribe_int();
}
