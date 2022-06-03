#include "rtc.h"
#include <lcom/lcf.h>

static int hook_id = 3;
uint16_t time_counter = 0;

int rtc_subscribe_int(uint8_t *bit_no) {

  *bit_no = 8;
  hook_id = 8;
  if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, &hook_id) != OK) {
    printf("Error subscribing to RTC\n");
    return 1;
  }
  return 0;
}

int rtc_unsubscribe_int() {
  if (sys_irqrmpolicy(&hook_id) != OK) {
    printf("Error unsubscribing to RTC\n");
    return 1;
  }
  return 0;
}

int rtc_read_reg(uint8_t reg, uint8_t *data) {
  uint32_t temp;

  if (sys_outb(RTC_ADDRESS_REG, reg) != OK) {
    printf("Error setting the adress register\n");
    return 1;
  }

  if (sys_inb(RTC_DATA_REG, &temp) != OK) {
    printf("Error reading the register\n");
    return 1;
  }
  *data = (uint8_t) temp;
  return 0;
}

int rtc_write_reg(uint8_t reg, uint8_t data) {
  uint32_t temp = (uint32_t) data;

  if (sys_outb(RTC_ADDRESS_REG, reg) != OK) {
    printf("Error setting the adress register\n");
    return 1;
  }

  if (sys_outb(RTC_DATA_REG, temp) != OK) {
    printf("Error writing to the register\n");
    return 1;
  }
  return 0;
}

int read_hours(uint8_t *hours) {
  if (rtc_read_reg(RTC_HOURS, hours) != OK) {
    return 1;
  }
  return 0;
}
int read_minutes(uint8_t *minutes) {
  if (rtc_read_reg(RTC_MINUTES, minutes) != OK) {
    return 1;
  }
  return 0;
}
