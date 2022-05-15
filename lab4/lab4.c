// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <mouse.h>
#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(mouse_test_packet)(uint32_t cnt) {
  struct packet pp;
  extern uint8_t byteFromMouse;
  uint8_t counter = 0;
  uint8_t bit_no;
  mouse_enable_data_reporting();
  mouse_subscribe_int(&bit_no);
  int r, ipc_status;
  message msg;
  while (cnt > 0) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 /* hardware interrupt notification */
          if (msg.m_notify.interrupts & BIT(bit_no)) { /* subscribed interrupt */
            mouse_ih();
            if (!(counter == 0 && (byteFromMouse & BIT(3)) == 0)) {
              pp.bytes[counter] = byteFromMouse;
              counter++;
            }
            if (counter == 3) {
              counter = 0;
              cnt--;
              parse_mouse_bytes(&pp);
              mouse_print_packet(&pp);
            }
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
  mouse_unsubscribe_int();
  disableDataReporting();
  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {
  struct packet pp;
  uint8_t init = idle_time;
  extern int totalInterrupts;
  extern uint8_t byteFromMouse;
  uint8_t counter = 0;
  uint8_t bit_no, bit_noTIMER;
  timer_subscribe_int(&bit_noTIMER);
  mouse_enable_data_reporting();
  mouse_subscribe_int(&bit_no);
  int r, ipc_status;
  message msg;
  while (idle_time > 0) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & BIT(bit_noTIMER)) {
            timer_int_handler();
            if (totalInterrupts % 60 == 0) {
              idle_time--;
            }
          }
          if (msg.m_notify.interrupts & BIT(bit_no)) { /* subscribed interrupt */
            idle_time = init;
            totalInterrupts = 0;
            mouse_ih();
            if (!(counter == 0 && (byteFromMouse & BIT(3)) == 0)) {
              pp.bytes[counter] = byteFromMouse;
              counter++;
            }
            if (counter == 3) {
              counter = 0;
              parse_mouse_bytes(&pp);
              mouse_print_packet(&pp);
            }
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

  mouse_unsubscribe_int();
  disableDataReporting();
  return 0;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  uint8_t bit_no, counter;
  struct packet pp;
  extern uint8_t byteFromMouse;
  int started = 0, peak = 0;
  mouse_subscribe_int(&bit_no);
  mouse_enable_data_reporting();

  int r, ipc_status;
  message msg;
  while (!(started && peak)) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 /* hardware interrupt notification */
          if (msg.m_notify.interrupts & BIT(bit_no)) { /* subscribed interrupt */
            mouse_ih();
            if (!(counter == 0 && (byteFromMouse & BIT(3)) == 0)) {
              pp.bytes[counter] = byteFromMouse;
              counter++;
            }
            if (counter == 3) {
              counter = 0;
              parse_mouse_bytes(&pp);
              mouse_print_packet(&pp);
            }
          }

          if (pp.lb && !pp.rb && !pp.mb) { // DRAWING LEFT LINE
            if (started) {
              if ((pp.delta_y + tolerance) / (double) (pp.delta_x - tolerance) <= 1 || (pp.delta_y - tolerance) / (double) (pp.delta_x + tolerance) <= 1) {
                started = 0;
              }
            }
            else {
              started += pp.delta_x;
            }
          }
          if (!pp.lb && !pp.mb && !pp.rb && started > 0 && started < x_len) {
            started = 0;
          }
          if (pp.rb && !pp.lb && !pp.mb) { // DRAWING LEFT LINE
            if (peak) {
              if ((pp.delta_y + tolerance) / (double) (pp.delta_x - tolerance) <= -1 || (pp.delta_y - tolerance) / (double) (pp.delta_x + tolerance) <= -1) {
                peak = 0;
                started = 0;
              }
            }
            else {
              peak += pp.delta_x;
            }
          }
          else {
            started = 0;
            peak = 0;
          }
          if (!pp.lb && !pp.mb && !pp.rb && peak > 0 && peak < x_len) {
            started = 0;
            peak = 0;
          }
          if (peak && started) {
            printf("GREAT SUCCESS\n");
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
  mouse_unsubscribe_int();
  disableDataReporting();
  return 0;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  /* This year you need not implement this. */
  printf("%s(%u, %u): under construction\n", __func__, period, cnt);
  return 1;
}
