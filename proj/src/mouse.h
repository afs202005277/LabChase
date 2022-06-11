#ifndef __MOUSE_H
#define __MOUSE_H
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>
#include <util.h>

/** @defgroup mouse mouse
 * @{
 *
 * Functions for using the mouse.
 */

#define PACKET_SIZE 3 /**< @brief Number of bytes of each mouse packet */

/**
 * @brief Subscribes the interrupts sent by the mouse (in exclusive mode).
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return 0 upon success, non-zero otherwise
 */
int(mouse_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Get the status byte from the KBC
 *
 * @param status memory address used to return the status
 * @return 0 upon success, non-zero otherwise
 */
int get_kbc_status(uint8_t *status);

/**
 * @brief Unsubscribes the mouse's interrupts
 *
 */
int(mouse_unsubscribe_int)();

/**
 * @brief reads a byte from the KBC's output buffer
 *
 * @param result memory address used to return the byte
 * @return 0 upon success, non-zero otherwise
 */
int read_output_buffer(uint8_t *result);

/**
 * @brief mouse interrupt handler
 *
 * Reads one byte from the KBC output buffer.
 * If there was any error in the KBC, the byte is discarded.
 *
 */
void(mouse_ih)();

/**
 * @brief Parses the bytes sent by the mouse into a more usable way.
 *
 * @param pp Memory address of a struct to be filled with the parsed information
 *
 */
void(parse_mouse_bytes)(struct packet *pp);

/**
 * @brief Disables the data reporting.
 *
 * Sends the command 0xF5 to the mouse.
 *
 */
void(disable_data_reporting)();

/**
 * @brief Enables data reporting.
 *
 * Sends the command
 *
 */
void(mouse_enable_reporting)();

/**
 * @brief sends a byte to the given port
 *
 * @param port the memory address of the port that will receive the byte
 * @param byte the byte to be sent
 * @return 0 upon success, non-zero otherwise
 */
int send_byte_to_KBC(uint8_t port, uint8_t byte);

/**
 * @brief sends a command to the KBC command register
 *
 * @param command command to be sent
 * @return 0 upon success, non-zero otherwise
 */
int send_cmd_to_KBC(uint8_t command);

/**
 * @brief sends a argument of a command to the KBC's input buffer
 *
 * @param argument argument to be sent
 * @return 0 upon success, non-zero otherwise
 */
int send_argument_to_KBC(uint8_t argument);

/**
 * @brief reads the mouse's response
 *
 * @param ack memory address to be filled with the response
 * @return 0 upon success, non-zero otherwise
 */
int read_ack_mouse(uint8_t *ack);

/**
 * @brief sends a command to the mouse
 *
 * Keeps sending the command until the mouse returns an error or a successfull response
 *
 * @param command command to be sent
 * @return 0 upon success, non-zero otherwise
 */
int send_cmd_to_mouse(uint8_t command);

/**
 * @brief enables the data reporting
 *
 * Changes the mouse operating mode to stream mode and enabes data reporting
 *
 */
void(mouse_enable_reporting)();

/**
 * @brief Get the received byte
 *
 * @return the received byte
 */
uint8_t get_received_byte();

/**@}*/
#endif /* __MOUSE_H */
