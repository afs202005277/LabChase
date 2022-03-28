#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include "keyboard.h"

#define OUT_BUF    0x60
#define STATUS     0x64
#define KBC_PAR_ERR BIT(7)
#define KBC_TO_ERR BIT(6)


uint8_t scanCode = 0;
int hook_id = 1;

int(keyboard_subscribe_int)(uint8_t *bit_no) {
  *bit_no = 1;
  return sys_irqsetpolicy(1, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
}

int(keyboard_unsubscribe_int)(){
  return sys_irqrmpolicy(&hook_id);
}

void(kbc_ih)(){
  uint8_t status;
  util_sys_inb(STATUS, &status);
  if ( (status &(KBC_PAR_ERR | KBC_TO_ERR)) == 0){
    util_sys_inb(OUT_BUF, &scanCode);
  }
}
