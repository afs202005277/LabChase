#include "keyboard.h"
#include "auxiliary_data_structures.h"
#include <lcom/lab3.h>
#include <lcom/lcf.h>

uint8_t scanCode = 0;
struct MovementInfo nextMove = {UNCHANGED, ME};
int hook_idKEYBOARD = 1;

int(send_KBC_command_byte)(uint32_t cmd) {
  uint8_t counter = 0, stat;
  while (counter < 10) {
    util_sys_inb(KBC_ST_REG, &stat);
    if ((stat & KBC_ST_IBF) == 0) {
      sys_outb(KBC_CMD_REG, cmd);
      return 0;
    }
    tickdelay(micros_to_ticks(DELAY_US));
    counter++;
  }
  return 1;
}

int(read_command_byte)(uint8_t *command) {
  return util_sys_inb(KBC_OUT_BUF, command);
}

int(write_command_byte)(uint8_t command) {
  return sys_outb(KBC_OUT_BUF, command);
}

int(keyboard_subscribe_int)(uint8_t *bit_no) {
  *bit_no = KBD_IRQ;
  return sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_idKEYBOARD);
}

int(keyboard_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_idKEYBOARD);
}

void(enableInterrupts)() {
  uint8_t command;
  send_KBC_command_byte(READ_CMD_BYTE);
  read_command_byte(&command);
  send_KBC_command_byte(WRITE_CMD_BYTE);
  write_command_byte(command | INT_KBC);
}

void(kbc_ih)() {
  uint8_t status, temp;
  util_sys_inb(KBC_ST_REG, &status);
  util_sys_inb(KBC_OUT_BUF, &temp);
  if ((status & LSB) == 1 && (status & (KBC_PAR_ERR | KBC_TO_ERR | KBC_AUX)) == 0) {

    if (temp == 153) {
      extern enum screenState screenState;
      if (screenState == PAUSE)
        screenState = S_GAME;
      else
        screenState = PAUSE; // Multiplayer
    }
    else {
      switch (temp) {
        case 145:
          nextMove.dir = UP;
          nextMove.playerID = ME;
          break;
        case 159:
          nextMove.dir = DOWN;
          nextMove.playerID = ME;
          break;
        case 158:
          nextMove.dir = LEFT;
          nextMove.playerID = ME;
          break;
        case 160:
          nextMove.dir = RIGHT;
          nextMove.playerID = ME;
          break;
        case 200:
          nextMove.dir = UP;
          nextMove.playerID = OTHER;
          break;
        case 208:
          nextMove.dir = DOWN;
          nextMove.playerID = OTHER;
          break;
        case 203:
          nextMove.dir = LEFT;
          nextMove.playerID = OTHER;
          break;
        case 205:
          nextMove.dir = RIGHT;
          nextMove.playerID = OTHER;
          break;
      }
    }
  }
  scanCode = temp;
}
