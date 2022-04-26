#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include "keyboard.h"


uint8_t scanCode = 0;
int hook_idKEYBOARD = 1;

int(send_KBC_command_byte)(uint32_t cmd){
  uint8_t counter=0, stat;
  while( counter < 10 ) {
    util_sys_inb(KBC_ST_REG, &stat); // Read the STATUS to check if IBF is full
    if ((stat & KBC_ST_IBF) == 0){
      sys_outb(KBC_CMD_REG, cmd); // If IBF bit is not set, send the command
      return 0;
    }
    tickdelay(micros_to_ticks(DELAY_US)); // If IBF is full, then the function waits some time
    counter++; // The function only waits 10 times
  }
  return 1;
}

int(read_command_byte)(uint8_t *command){
  return util_sys_inb(KBC_OUT_BUF, command);
}

int(write_command_byte)(uint8_t command){
  return sys_outb(KBC_OUT_BUF, command);
}

int(keyboard_subscribe_int)(uint8_t *bit_no) {
  *bit_no = KBD_IRQ;
  return sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_idKEYBOARD);
}

int(keyboard_unsubscribe_int)(){
  return sys_irqrmpolicy(&hook_idKEYBOARD);
}

void(enableInterrupts)(){
  uint8_t command;
  send_KBC_command_byte(READ_CMD_BYTE);
  read_command_byte(&command);
  send_KBC_command_byte(WRITE_CMD_BYTE);
  write_command_byte(command | INT_KBC);
}

void(kbc_ih)(){
  uint8_t status, temp;
  util_sys_inb(KBC_ST_REG, &status);
  util_sys_inb(KBC_OUT_BUF, &temp);
  if ( (status & LSB) == 1 && (status & (KBC_PAR_ERR | KBC_TO_ERR | KBC_AUX)) == 0){
    scanCode = temp;
  }
}
