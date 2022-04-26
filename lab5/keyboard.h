#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include <util.h>

// Auxiliary macros:
#define MSB BIT(7)
#define LSB BIT(0)
#define ESC_BREAK_CODE 0x81
#define KBD_IRQ 1
#define DELAY_US 20000 // delay (in micro seconds)
#define INT_KBC 0x01 // bit mask to enable the interrupts of the keyboard

// KBC commands: 
#define READ_CMD_BYTE 0x20
#define WRITE_CMD_BYTE 0x60

// KBC ports:
#define KBC_OUT_BUF 0x60
#define KBC_ST_REG 0x64
#define KBC_CMD_REG 0x64

// KBC bit masks:
#define KBC_PAR_ERR BIT(7) // Parity error
#define KBC_ST_IBF BIT(1)  // IBF bit set (IBF full)
#define KBC_OUT_FULL BIT(0) // OBF bit set (OBF full => data available for reading)
#define KBC_TO_ERR BIT(6)   // Time out error
#define KBC_AUX BIT(5)      // Aux bit (set if there is input from mouse)

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

int (send_KBC_command_byte)(uint32_t cmd);

int (read_command_byte)(uint8_t *command);

int (write_command_byte)(uint8_t command);

void (enableInterrupts)();


#endif /* __KEYBOARD_H */
