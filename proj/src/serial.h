#ifndef __SERIAL_H
#define __SERIAL_H

#define COM1_PORT 0x3F8
#define COM1_IRQ 4
#define IER 1
#define LCR 3
#define FCR 2


int serial_subscribe(uint8_t* bit_no);

int serial_unsubscribe();

#endif