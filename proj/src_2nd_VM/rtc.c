#include "rtc.h"
#include <lcom/lcf.h>

int rtc_read_reg(uint8_t reg, uint8_t *data) {
  uint32_t temp;

  if (sys_outb(RTC_ADDRESS_REG, reg) != OK) {
    printf("Error setting the adress register\n");
    return 1;
  }

  if (sys_inb(RTC_DATA_REG, &temp) != OK) {
    printf("Error reading the register\n");
    return 2;
  }
  *data = (uint8_t) temp;
  return 0;
}

int read_hours(uint8_t *hours) {
  if (rtc_read_reg(RTC_HOURS, hours) != OK) {
    return 1;
  }
  return 0;
}
