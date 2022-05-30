#include <lcom/lcf.h>
#include "serial.h"

int serial_hook = COM1_IRQ;

int write_to_IER(uint8_t byte){
  uint8_t wordInLCR;
  read_from_LCR(&wordInLCR);
  write_to_LCR(wordInLCR & 0x7F); // set DLAB = 0
  return sys_outb(COM1_PORT + IER, byte);;
}

int write_to_LCR(uint8_t byte){
  return sys_outb(COM1_PORT + LCR, byte);
}

int read_from_LCR(uint8_t* byte){
  return util_sys_inb(COM1_PORT + LCR, byte);
}

int write_to_FCR(uint8_t byte){
  return sys_outb(COM1_PORT + FCR, byte);
}

int serial_subscribe(uint8_t* bit_no) {
  uint8_t enableInterrupts = 1; // enables received data available interrupt
  uint8_t byteInLCR;
  write_to_IER(enableInterrupts);
  read_from_LCR(&byteInLCR);
  write_to_LCR(byteInLCR | BIT(1) | BIT(0)); // set word of 8 bits
  write_to_FCR(BIT(2) | BIT(1) | BIT(0)); // clears the fifos and enables them

  *bit_no = serial_hook;
  sys_irqsetpolicy(COM1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &serial_hook);
  return 0;
}

int serial_unsubscribe() {
  if (sys_irqrmpolicy(&serial_hook) != 0)
    return -1;
  return 0;
}