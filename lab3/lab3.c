#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include <util.h>
#include "keyboard.h"

extern uint8_t scanCode;
extern int countSysInb;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  uint8_t bit_no;
  keyboard_subscribe_int(&bit_no);
  int r, ipc_status;
  message msg;
  scanCode = 0;
  countSysInb = 0;
  while( scanCode != ESC_BREAK_CODE ) {
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
                if (msg.m_notify.interrupts & BIT(bit_no)) { /* subscribed interrupt */
                  kbc_ih();   /* process it */
                  kbd_print_scancode((scanCode & MSB) == 0, 1, &scanCode);
                }
                break;
            default:
                break; /* no other notifications expected: do nothing */	
        }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
  }
  keyboard_unsubscribe_int();
  kbd_print_no_sysinb(countSysInb);
  return 0;
}


int(kbd_test_poll)() {
  scanCode = 0;
  countSysInb = 0;
  while (scanCode != ESC_BREAK_CODE) { 
    scanCode = 0;
    kbc_ih();
    if (scanCode != 0) {
      kbd_print_scancode((scanCode & MSB) == 0, 1, &scanCode);
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  enableInterrupts();
  kbd_print_no_sysinb(countSysInb);
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t nInit=n;
  message msg;
  int ipc_status, r;
  uint8_t bit_noTIMER=0, bit_noKBC=0;
  extern int totalInterrupts;
  timer_subscribe_int(&bit_noTIMER);
  keyboard_subscribe_int(&bit_noKBC);
  while( scanCode != ESC_BREAK_CODE && n > 0) {
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
                if (msg.m_notify.interrupts & BIT(bit_noKBC)) { /* subscribed interrupt */
                  kbc_ih();   /* process it */
                  kbd_print_scancode((scanCode & MSB) == 0, 1, &scanCode);
                  n = nInit;
                  totalInterrupts=0;
                }
                if (msg.m_notify.interrupts & BIT(bit_noTIMER)){
                  timer_int_handler();
                  if (totalInterrupts % 60 == 0)
                    n--;
                }
                break;
            default:
                break; /* no other notifications expected: do nothing */	
        }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
  }
  timer_unsubscribe_int();
  keyboard_unsubscribe_int();
  return 0;
}
