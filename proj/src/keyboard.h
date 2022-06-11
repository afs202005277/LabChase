#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include <util.h>
#include "auxiliary_data_structures.h"

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

/**
 * @brief Makes the interpretation of the keycode received and returns the corresponding movement
 *
 * @param screenState memory address of the screen state (needed if the user pressed the pause button)
 * @return struct MovementInfo struct that contains the interpretation of the keycode
 */
struct MovementInfo key_code_interpreter(enum screenState *screenState);

/**@}*/
#endif /* __KEYBOARD_H */
