#ifndef __SERIAL_H
#  define __SERIAL_H

#  include "Ball.h"
#  include "Menu.h"
#  include "goalkeeper.h"
#  include <minix/drivers.h>
#  include <minix/syslib.h>

#  define BIT(n) (0x01 << (n))

#  define COM1_PORT 0x3F8
#  define COM2_PORT 0x2F8
#  define COM1_IRQ 4
#  define COM2_IRQ 3

#  define DELAY 10000

#  define SERIAL_BITRATE 1200 // STANDARD
#  define SERIAL_BASE_FREQ 115200

// PENALTIX
#  define AREA 0x40
#  define X_SHOOT 0x41
#  define Y_SHOOT 0x42
#  define END_X_SHOOT 0x43
#  define END_Y_SHOOT 0x44
#  define KEY_MATCH 0x45

// ADRESS OF UART REGISTERS
#  define RECEIVER_BUFFER_READ 0x00
#  define TRANSMITTER_HOLDING_WRITE 0x00
#  define INTERRUPT_ENABLE 0x01 // WRITE AND READ
#  define INTERRUPT_ID_READ 0x02
#  define FIFO_CONTROL_REGISTER_READ 0x02
#  define LINE_CONTROL 0x03  // WRITE AND READ
#  define MODEM_CONTROL 0x04 // WRITE AND READ
#  define LINE_STATUS_READ 0x05
#  define MODEM_STATUS_READ 0x06
#  define SCRATCHPAD 0x07 // WRITE AND READ

#  define DIVISOR_LATCH_LSB 0x00 // WRITE AND READ
#  define DIVISOR_LATCH_MSB 0x01 // WRITE AND READ

// LINE CONTROL REGISTER
// BITS PER CHAR
#  define FIVE 0
#  define SIX BIT(0)
#  define SEVEN BIT(1)
#  define EIGHT BIT(1) | BIT(0)
// STOP BITS
#  define ONE_BIT 0
#  define TWO_BITS BIT(2)
// PARITY CONTROL
#  define NO_PARITY 0
#  define ODD_PARITY BIT(3)
#  define EVEN_PARITY (BIT(4) | BIT(3))
#  define ALWAYS_1_PARITY (BIT(5) | BIT(3))
#  define ALWAYS_0_PARITY (BIT(5) | BIT(4) | BIT(3))
// BREAK CONTROL
#  define BREAK_CONTROL BIT(6)
// DIVISOR LATCH ACESS
#  define DLAB BIT(7)
#  define DATA 0

// LINE STATUS REGISTER (READ)
/* LSR read -> Bit 0 to 4 are reset */
#  define RECEIVER_DATA BIT(0)
#  define OVERRUN_ERROR BIT(1)
#  define PARITY_ERROR BIT(2)
#  define FRAMING_ERROR BIT(3)
#  define BREAK_INTERRUPT BIT(4)
#  define TH_EMPTY BIT(5)
#  define BOTH_EMPTY BIT(6)
#  define FIFO_ERROR BIT(7)

// INTERRUPT ENABLE REGISTER
#  define ENABLE_INTERRUPT_DATA BIT(0)
#  define ENABLE_INTERRUPT_TH BIT(1)
#  define ENABLE_INTERRUPT_LS BIT(2)
#  define ENABLE_INTERRUPT_MODEM BIT(3)

// INTERRUPT IDENTIFICATION REGISTER

#  define NO_INTERUPTS BIT(0)
#  define TH_IS_EMPTY BIT(1)
#  define DATA_AVAILABLE BIT(2)
#  define RL_STATUS BIT(2) | BIT(1)
#  define CHAR_TIMEOUT BIT(3)
#  define FIFO_64BYTE BIT(5)
#  define NO_FIFO 0
#  define FIFO_UNUSABLE BIT(7)
#  define FIFO_ENABLED BIT(7) | BIT(6)

/**
 * @brief Configure Line Control Register, subscribes UART and enable interrupts from UART.
 * @return Return 0 upon success and negative otherwise.
 */
int serial_subscribe();

/**
 * @brief Unsubscribes UART and disable interrupts from UART.
 * @return Return 0 upon success and negative otherwise.
 */
int serial_unsubscribe();
