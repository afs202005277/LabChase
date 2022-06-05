#ifndef __SERIAL_H
#define __SERIAL_H
#include <stdint.h>
#include <stdbool.h>

#define COM1_PORT 0x3F8
#define COM1_IRQ 4
#define IER 1
#define LCR 3
#define FCR 2
#define LSR 5
#define THR 0
#define RBR 0
#define IIR 2
#define MAX_WAIT 5
#define SERIAL_DELAY 2000

#define CAN_WRITE_TO_THR BIT(5)
#define DATA_AVAILABLE BIT(0)

int receive_connection(bool* connection);

int serial_subscribe(uint8_t* bit_no);

int serial_unsubscribe();

int send_character(uint8_t character);

int read_character(uint8_t* character);

void serial_ih();

int clear_DLAB();

int write_to_IER(uint8_t byte);

int write_to_LCR(uint8_t byte);

int read_from_LCR(uint8_t *byte);

int write_to_FCR(uint8_t byte);

int write_to_THR(uint8_t byte);

int read_LSR(uint8_t *byte);

int read_RBR(uint8_t *byte);

int read_IIR(uint8_t* byte);

int send_character(uint8_t character);

int read_character(uint8_t* character);

void serial_ih();

#endif
