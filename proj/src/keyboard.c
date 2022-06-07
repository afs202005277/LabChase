#include "keyboard.h"
#include "auxiliary_data_structures.h"
#include "kbc_macros.h"
#include "key_codes.h"
#include <lcom/lab3.h>
#include <lcom/lcf.h>

uint8_t scanCode = 0;
struct MovementInfo nextMove = {UNCHANGED, ME};
static int hook_id_keyboard = KBD_IRQ;

int(keyboard_subscribe_int)(uint8_t *bit_no) {
  *bit_no = KBD_IRQ;
  return sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_keyboard);
}

int(keyboard_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id_keyboard);
}

void(kbc_ih)() {
  extern enum screenState screenState;
  uint8_t status, temp;
  util_sys_inb(KBC_ST_REG, &status);
  util_sys_inb(KBC_OUT_BUF, &temp);
  if ((status & KBC_OUT_FULL) == 1 && (status & (KBC_PAR_ERR | KBC_TO_ERR | KBC_AUX)) == 0) {

    if (temp == 153 && screenState != M_GAME) {
      if (screenState == PAUSE)
        screenState = S_GAME;
      else
        screenState = PAUSE;
    }

    nextMove.playerID = ME;
    switch (temp) {
      case UP_PLAYER1_BR:
        printf("UP\n");
        nextMove.dir = UP;
        break;
      case DOWN_PLAYER1_BR:
        printf("DOWN\n");
        nextMove.dir = DOWN;
        break;
      case LEFT_PLAYER1_BR:
        printf("LEFT\n");
        nextMove.dir = LEFT;
        break;
      case RIGHT_PLAYER1_BR:
        printf("RIGHT\n");
        nextMove.dir = RIGHT;
        break;
    }
    if (screenState == S_GAME) {
      switch (temp) {
        case UP_PLAYER2_BR:
          nextMove.playerID = OTHER;
          nextMove.dir = UP;
          break;
        case DOWN_PLAYER2_BR:
          nextMove.playerID = OTHER;
          nextMove.dir = DOWN;
          break;
        case LEFT_PLAYER2_BR:
          nextMove.playerID = OTHER;
          nextMove.dir = LEFT;
          break;
        case RIGHT_PLAYER2_BR:
          nextMove.playerID = OTHER;
          nextMove.dir = RIGHT;
          break;
      }
    }
  }
  scanCode = temp;
}
