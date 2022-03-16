#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  uint16_t mask = 0x00FF;
  *lsb = mask & val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  uint16_t mask = 0xFF00;
  *msb = (mask & val) >> 8;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t b;
  sys_inb(port, &b);
  *value = (uint8_t) b;
  return 0;
}
