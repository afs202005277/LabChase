#include "serial.h"
#include <lcom/lcf.h>

int serial_hook = COM1_IRQ;

uint8_t receivedChar = 0;

int receive_connection(bool* connection){
  if (receivedChar == 'k'){
    *connection = true;
    send_character('l');
  }
  return 0;
}

int clear_DLAB(){
  uint8_t wordInLCR;
  read_from_LCR(&wordInLCR);
  write_to_LCR(wordInLCR & 0x7F); // set DLAB = 0
  return 0;
}

int write_to_IER(uint8_t byte) {
  clear_DLAB();
  return sys_outb(COM1_PORT + IER, byte);
}

int write_to_LCR(uint8_t byte) {
  return sys_outb(COM1_PORT + LCR, byte);
}

int read_from_LCR(uint8_t *byte) {
  return util_sys_inb(COM1_PORT + LCR, byte);
}

int write_to_FCR(uint8_t byte) {
  return sys_outb(COM1_PORT + FCR, byte);
}

int write_to_THR(uint8_t byte){
  return sys_outb(COM1_PORT+THR, byte);
}

int serial_subscribe(uint8_t *bit_no) {
  uint8_t enableInterrupts = 1; // enables received data available interrupt
  uint8_t byteInLCR;
  write_to_IER(enableInterrupts);
  read_from_LCR(&byteInLCR);
  write_to_LCR(byteInLCR | BIT(1) | BIT(0)); // set word of 8 bits
  write_to_FCR(BIT(2) | BIT(1) | BIT(0));    // clears the fifos and enables them

  *bit_no = serial_hook;
  sys_irqsetpolicy(COM1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &serial_hook);
  return 0;
}

int serial_unsubscribe() {
  if (sys_irqrmpolicy(&serial_hook) != 0)
    return -1;
  return 0;
}

int read_LSR(uint8_t *byte) {
  return util_sys_inb(COM1_PORT + LSR, byte);
}

int read_RBR(uint8_t *byte){
  clear_DLAB();
  return util_sys_inb(COM1_PORT + RBR, byte);
}

int send_character(uint8_t character) {
  uint8_t numTimesWaited = 0;
  uint8_t lsr;
  while (numTimesWaited < MAX_WAIT) {
      read_LSR(&lsr);
      if ((lsr & CAN_WRITE_TO_THR) != 0){
        return write_to_THR(character);
      }
      tickdelay(micros_to_ticks(SERIAL_DELAY));
      numTimesWaited++;
  }
  printf("Waited too many times to send character.\n");
  return 1;
}

int read_character(uint8_t* character){
  uint8_t status;
  do{
    read_LSR(&status);
    if ((status & DATA_AVAILABLE) != 0){
      read_RBR(character);
    }
  } while((status & DATA_AVAILABLE) != 0);
  return 0;
}

void serial_ih(){
  uint8_t tmp;
  read_character(&tmp);
  receivedChar = tmp;
}


