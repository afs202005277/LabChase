#ifndef __SERIAL_H
#define __SERIAL_H
#include <stdbool.h>
#include <stdint.h>

/** @defgroup serial serial
 * @{
 *
 * Functions for using the serial port
 */

#define COM1_PORT 0x3F8   /**< @brief COM1 base address */
#define COM1_IRQ 4        /**< @brief COM1 IRQ line */
#define IER 1             /**< @brief IER register offset relative to the COM1 base address */
#define LCR 3             /**< @brief LCR register offset relative to the COM1 base address */
#define FCR 2             /**< @brief FCR register offset relative to the COM1 base address */
#define LSR 5             /**< @brief LSR register offset relative to the COM1 base address */
#define THR 0             /**< @brief THR register offset relative to the COM1 base address */
#define RBR 0             /**< @brief RBR register offset relative to the COM1 base address */
#define MAX_WAIT 5        /**< @brief Maximum number of times waited to send a byte (used in polling mode) */
#define SERIAL_DELAY 2000 /**< @brief Delay between consecutive tries to send a byte in polling mode */

#define CAN_WRITE_TO_THR BIT(5) /**< @brief Bit mask to check if bit 5 of LSR is set (if set, can write to THR)*/
#define DATA_AVAILABLE BIT(0)   /**< @brief Bit mask to check if bit 0 of LSR is set (if set, there is data available for reading) */

/**
 * @brief Subscribes the interrupts sent by the serial port (in exclusive mode).
 *
 * Enables Received Data Available Interrupt
 * Sets whe word size (8 bits)
 * Clears the FIFO's contents and enables them
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return 0 upon success, non-zero otherwise
 */
int serial_subscribe(uint8_t *bit_no);

/**
 * @brief Unsubscribes the interrupts from serial port
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int serial_unsubscribe();

/**
 * @brief Sends a byte through the serial port, using polling
 * to check if the Transmitter Holding Register is empty
 *
 * The function tries to send the byte a limited number of times and waits
 * 2ms between consecutive tries
 *
 * @param character the byte to be sent
 * @return Return 0 upon success and non-zero otherwise
 */
int send_character(uint8_t character);

/**
 * @brief Reads a character from the Receiver Buffer Register
 * Used by the interrupt handler to get the received byte
 *
 * @param character address of memory used to return the byte received trought the serial port
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int read_character(uint8_t *character);

/**
 * @brief Serial port interrupt handler
 *
 * Reads a byte from the Receiver Buffer Register
 *
 */
void serial_ih();

/**
 * @brief Sets the DLAB flag (bit 7 of the LCR) to 0
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int clear_DLAB();

/**
 * @brief Writes a given byte to the Interrupt Enable Register
 *
 * Used to enable the desired interrupts from the serial port
 *
 * @param byte byte to be written
 * @return Return 0 upon success and non-zero otherwise
 */
int write_to_IER(uint8_t byte);

/**
 * @brief Writes a given byte to the Line Control Register
 *
 * Used to select the word size and to change the value of the DLAB flag
 *
 * @param byte byte to be written
 * @return Return 0 upon success and non-zero otherwise
 */
int write_to_LCR(uint8_t byte);

/**
 * @brief Reads the word present in the Line Control Register
 *
 * Used to get the byte stored in the Line Control Register
 *
 * @param byte memory address to return the word
 * @return Return 0 upon success and non-zero otherwise
 */
int read_from_LCR(uint8_t *byte);

/**
 * @brief Writes a byte to the FIFO Control Register
 *
 * Used to program the FIFO's and to reset their contents
 *
 * @param byte byte to be written
 * @return Return 0 upon success and non-zero otherwise
 */
int write_to_FCR(uint8_t byte);

/**
 * @brief Writes a byte in the Transmitter Holding Register
 *
 * Used to send a byte through the serial port
 *
 * @param byte byte to be written
 * @return Return 0 upon success and non-zero otherwise
 */
int write_to_THR(uint8_t byte);

/**
 * @brief Reads a byte from the Line Status Register
 *
 * Used to get the information about the status of the THR (empty or not).
 *
 * @param byte memory address used to return the byte in the LSR
 * @return Return 0 upon success and non-zero otherwise
 */
int read_LSR(uint8_t *byte);

/**
 * @brief Reads a byte from the Receiver Buffer Register
 *
 * Used to get the character received by the serial port
 *
 * @param byte memory address used to return the byte read from the RBR
 * @return Return 0 upon success and non-zero otherwise
 */
int read_RBR(uint8_t *byte);

/**@}*/
#endif /* __SERIAL_H */
