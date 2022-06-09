#include "keyboard.h"
#include "auxiliary_data_structures.h"
#include "kbc_macros.h"
#include "key_codes.h"
#include <lcom/lab3.h>
#include <lcom/lcf.h>

static int hook_id_keyboard = KBD_IRQ;
static uint8_t keycode;

int(keyboard_subscribe_int)(uint8_t *bit_no) {
  *bit_no = KBD_IRQ;
  return sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_keyboard);
}

int(keyboard_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id_keyboard);
}

struct MovementInfo key_code_interpreter() {
  struct MovementInfo nextMove = {UNCHANGED, ME};
  kbc_ih();
  extern enum screenState screenState;
  if (keycode == PAUSE_BUTTON_BR && screenState != M_GAME) {
    if (screenState == PAUSE)
      screenState = S_GAME;
    else
      screenState = PAUSE;
  }
  else {
    switch (keycode) {
      case UP_PLAYER1_BR:
        nextMove.dir = UP;
        break;
      case DOWN_PLAYER1_BR:
        nextMove.dir = DOWN;
        break;
      case LEFT_PLAYER1_BR:
        nextMove.dir = LEFT;
        break;
      case RIGHT_PLAYER1_BR:
        nextMove.dir = RIGHT;
        break;
    }
    if (screenState == S_GAME) {
      switch (keycode) {
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
  return nextMove;
}

void(kbc_ih)() {
  uint8_t status, temp;
  util_sys_inb(KBC_ST_REG, &status);
  util_sys_inb(KBC_OUT_BUF, &temp);
  if ((status & KBC_OUT_FULL) == 1 && (status & (KBC_PAR_ERR | KBC_TO_ERR | KBC_AUX)) == 0) {
    keycode = temp;
  } else{
    keycode = KBD_ERROR_BYTE;
  }
}
