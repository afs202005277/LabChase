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
  mouse_subscribe_int(&bit_no);
  mouse_enable_data_reporting();
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
              // parse_mouse_bytes(&pp);

              pp.lb = pp.bytes[0] & BIT(0);
              pp.mb = pp.bytes[0] & BIT(2);
              pp.rb = pp.bytes[0] & BIT(1);
              pp.x_ov = pp.bytes[0] & BIT(6);
              pp.y_ov = pp.bytes[0] & BIT(7);

              if ((pp.bytes[0] & BIT(4)) == 0) { // X is positive
                pp.delta_x = pp.bytes[1];
              }
              else {
                pp.delta_x = -255 + pp.bytes[1];
              }
              if ((pp.bytes[0] & BIT(5)) == 0) { // Y is positive
                pp.delta_y = pp.bytes[2];
              }
              else {
                pp.delta_y = -255 + pp.bytes[2];
              }
              //printf("delta_x: %d\n", pp.delta_x);
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
  disableDataReporting();
  mouse_unsubscribe_int();
  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {
  /* To be completed */
  printf("%s(%u): under construction\n", __func__, idle_time);
  return 1;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  /* To be completed */
  printf("%s: under construction\n", __func__);
  return 1;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  /* This year you need not implement this. */
  printf("%s(%u, %u): under construction\n", __func__, period, cnt);
  return 1;
}
