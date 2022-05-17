#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int hook_id;
int totalInterrupts=0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  uint8_t initial_conf, port;
  timer_get_conf(timer, &initial_conf);
  initial_conf &= 0x0F; // selecionar 4 bits menos significativos
  uint8_t controlW = timer << 6 |  TIMER_LSB_MSB | initial_conf;
  sys_outb(TIMER_CTRL, controlW);
  if (timer == 0){
    port = TIMER_0;
  } else if (timer == 1) {
    port = TIMER_1;
  } else if (timer == 2) {
    port = TIMER_2;
  } else {
    return 1;
  }

  if (freq < 18){
    printf("INVALID FREQUENCY\n");
    return 1;
  }

  uint16_t divisor = TIMER_FREQ / freq;
  uint8_t LSB_divisor, MSB_divisor;
  util_get_LSB(divisor, &LSB_divisor);
  util_get_MSB(divisor, &MSB_divisor);

  sys_outb(port, LSB_divisor);
  sys_outb(port, MSB_divisor);

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = 0;
  hook_id = 0;
  sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id);
  return 0;
}

int (timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}

void (timer_int_handler)() {
 totalInterrupts++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint32_t ReadBack;
  if (timer == 0){
    ReadBack = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(0);
    sys_outb(TIMER_CTRL, ReadBack);
    util_sys_inb(TIMER_0, st);
    return 0;
  }
  else if (timer == 1){
    ReadBack = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(1);
    sys_outb(TIMER_CTRL, ReadBack);
    util_sys_inb(TIMER_1, st);
    return 0;
  }
  else if (timer == 2){
    ReadBack = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(2);
    sys_outb(TIMER_CTRL, ReadBack);
    util_sys_inb(TIMER_2, st);
    return 0;
  }
  return 1;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  union timer_status_field_val f;
  uint8_t mode=0;
  uint8_t countingMode = (st & TIMER_MODE) >> 1;
  switch (field)
  {
  case tsf_all:
    f.byte = st;
    break;
  case tsf_initial:
    mode = (st & TIMER_ACCESS) >> 4;
    f.in_mode = mode;
    break;
  case tsf_mode:
    if (countingMode == 2 || countingMode == 6)
      f.count_mode = 2;
    else if (countingMode == 3 || countingMode == 7)
      f.count_mode = 3;
    else
      f.count_mode = countingMode;
    break;
  case tsf_base:
    f.bcd = st & TIMER_BCD;
    break;
  }
  timer_print_config(timer, field, f);
  return 0;
}
