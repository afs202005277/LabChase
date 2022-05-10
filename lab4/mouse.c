#include "mouse.h"
#include "kbc_macros.h"
#include <lcom/lab4.h>
#include <lcom/lcf.h>
#include "keyboard.h"

uint8_t byteFromMouse;
int hook_idMOUSE = MOUSE_IRQ;

void(mouse_ih)() {
  uint8_t status, temp;
  util_sys_inb(KBC_ST_REG, &status);
  util_sys_inb(KBC_OUT_BUF, &temp);
  /*printf("%d\n", (status & KBC_PAR_ERR));
  printf("%d\n", (status & KBC_TO_ERR));
  printf("%d\n", (status & KBC_OUT_FULL));
  printf("%d\n", (status & KBC_AUX));*/
  if (((status & KBC_PAR_ERR) | (status & KBC_TO_ERR)) == 0 && (status & KBC_OUT_FULL) == 1 && (status & KBC_AUX) != 0) {
    //printf("INSIDE IH\n");
    byteFromMouse = temp;
  }
}
/*
void(parse_mouse_bytes)(struct packet* pp){
  pp->lb = pp->bytes[0] & BIT(0);
  pp->mb = pp->bytes[0] & BIT(2);
  pp->rb = pp->bytes[0] & BIT(1);
  pp->x_ov = pp->bytes[0] & BIT(6);
  pp->y_ov = pp->bytes[0] & BIT(7);

  if ((pp->bytes[0] & BIT(4)) == 0){ // X is positive
    pp->delta_x = pp->bytes[1];
  }
  else {
    pp->delta_x = -255 + pp->bytes[1];
  }
  if ((pp->bytes[0] & BIT(5)) == 0){ // Y is positive
    pp->delta_y = pp->bytes[2];
  } else{
    pp->delta_y = -255 + pp->bytes[2];
  }
}
*/
void(disableDataReporting)(){
  send_KBC_command_byte(0xD4);
  tickdelay(micros_to_ticks(27000));
  write_command_byte(0xF5);
}


int(mouse_subscribe_int)(uint8_t *bit_no){
  *bit_no = hook_idMOUSE;
  return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_idMOUSE);
}

int(mouse_unsubscribe_int)(){
  return sys_irqrmpolicy(&hook_idMOUSE);
}
