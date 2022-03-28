#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include <util.h>

#define OUT_BUF    0x60
#define STATUS     0x64
#define KBC_PAR_ERROR BIT(7)
#define KBC_TO_ERR BIT(6)

/**
 * @brief Subscribes and enables keyboard interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int(keyboard_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes keyboard interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(keyboard_unsubscribe_int)();

/**
 * @brief keyboard interrupt handler
 *
 * Increments counter
 */
void(kbc_ih)();


#endif /* __KEYBOARD_H */
