#include <lcom/lcf.h>

#include <stdint.h>

/** @defgroup utils utils
 * @{
 *
 * Functions for helping the game development.
 */

/**
 * @brief Computes the least significant byte of a 2 byte word
 *
 * @param val The original word
 *
 * @param lsb memory address used to return the least significant byte of the word
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  uint16_t mask = 0x00FF;
  *lsb = mask & val;
  return OK;
}

/**
 * @brief Computes the most significant byte of a 2 byte word
 *
 * @param val The original word
 *
 * @param msb memory address used to return the most significant byte of the word
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  uint16_t mask = 0xFF00;
  *msb = (mask & val) >> 8;
  return OK;
}

/**
 * @brief Reads 4 bytes from a given port and returns the least significant one
 *
 * @param port The port to read the bytes
 *
 * @param value memory address used to return the least significant byte of the 4 bytes word
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(util_sys_inb)(int port, uint8_t *value) {
  uint32_t b;
  if (sys_inb(port, &b) != 0)
    return 1;
  *value = (uint8_t) b;
  return OK;
}

/**
 * @brief Returns the minimum of two numbers
 * 
 * @param num1 
 * @param num2 
 * @return the minimum number
 */
unsigned int min(unsigned int num1, unsigned int num2) {
  if (num1 > num2)
    return num2;
  return num1;
}
