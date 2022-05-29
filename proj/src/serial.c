#include "serial.h"

int serial_hook;

int serial_subscribe() {
  serial_hook = 3;

  // PROGRAM LINE CONTROL REGISTER TO PENALTIX
  unsigned long r, enable = 0;
  sys_inb(COM1_PORT + LINE_CONTROL, &r);

  r &= BREAK_CONTROL;
  r |= DLAB;
  r |= EIGHT;
  r |= ONE_BIT;
  r |= NO_PARITY;

  sys_outb(COM1_PORT + LINE_CONTROL, r);
  serial_set_bitrate(SERIAL_BITRATE);

  // SUBSCRIBE INTERRUPTS
  if (sys_irqsetpolicy(COM1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &serial_hook) != 0)
    return -1;

  if (sys_irqenable(&serial_hook) != 0)
    return -1;

  // ENABLE RECEIVED DATA INTERRUPT AND RECEIVER LINE STATUS INTERRUPT
  sys_inb(COM1_PORT + INTERRUPT_ENABLE, &r);
  enable = ENABLE_INTERRUPT_DATA | ENABLE_INTERRUPT_LS;
  r |= enable;
  sys_outb(COM1_PORT + INTERRUPT_ENABLE, r);
  tickdelay(micros_to_ticks(DELAY));
  return 0;
}

int serial_unsubscribe() {

  // UNSUBSCRIBE INTERRUPTS
  if (sys_irqdisable(&serial_hook) != 0)
    return -1;

  if (sys_irqrmpolicy(&serial_hook) != 0)
    return -1;

  // DISABLE ALL INTERRUPTS FROM UART
  if (sys_outb(COM1_PORT + INTERRUPT_ENABLE, 0) != OK)
    return -1;
  return 0;
}