#include "mouse.h"
#include "kbc_macros.h"
#include "mouse_masks.h"
#include <lcom/lcf.h>

static uint8_t receivedByte;
static int hook_id_mouse = MOUSE_IRQ;

uint8_t get_received_byte(){
  return receivedByte;
}

int get_kbc_status(uint8_t *status) {
  return util_sys_inb(KBC_ST_REG, status);
}

int read_output_buffer(uint8_t *result) {
  return util_sys_inb(KBC_OUT_BUF, result);
}

void(mouse_ih)() {
  uint8_t status, tmp;
  get_kbc_status(&status);
  read_output_buffer(&tmp);
  if ((status & KBC_PAR_ERR) == 0 && (status & KBC_TO_ERR) == 0 && (status & KBC_AUX) != 0 && (status & KBC_OUT_FULL) == 1) {
    receivedByte = tmp;
  }
}

void(parse_mouse_bytes)(struct packet *pp) {
  pp->lb = pp->bytes[0] & LB;
  pp->rb = pp->bytes[0] & RB;
  pp->mb = pp->bytes[0] & MB;

  pp->x_ov = pp->bytes[0] & X_OV;
  pp->y_ov = pp->bytes[0] & Y_OV;

  uint8_t signalX = pp->bytes[0] & X_SIGNAL;
  uint8_t signalY = pp->bytes[0] & Y_SIGNAL;

  if (signalX == 0)
    pp->delta_x = pp->bytes[1];
  else
    pp->delta_x = -256 + pp->bytes[1];
  if (signalY == 0)
    pp->delta_y = pp->bytes[2];
  else
    pp->delta_y = -256 + pp->bytes[2];
}

int(mouse_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id_mouse;
  return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse);
}

int(mouse_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id_mouse);
}

int send_byte_to_KBC(uint8_t port, uint8_t byte){
  uint8_t status;
  uint8_t wait = 15;
  do {
    get_kbc_status(&status);
    if ((status & BIT(1)) == 0) {
      return sys_outb(port, byte);
    }
    tickdelay(micros_to_ticks(KEYBOARD_DELAY));
    wait--;
  } while (wait > 0);
  printf("Waited too many times.\n");
  return 1;
}

int send_cmd_to_KBC(uint8_t command) {
  return send_byte_to_KBC(KBC_CMD_REG, command);
}

int send_argument_to_KBC(uint8_t argument) {
  return send_byte_to_KBC(KBC_IN_BUF, argument);
}

int read_ack_mouse(uint8_t *ack) {
  return util_sys_inb(KBC_OUT_BUF, ack);
}

int send_cmd_to_mouse(uint8_t command){
  uint8_t ack;
  do {
    send_cmd_to_KBC(WRITE_TO_MOUSE);
    send_argument_to_KBC(command);
    tickdelay(micros_to_ticks(MOUSE_DELAY));
    read_ack_mouse(&ack);
  } while (ack == NACK);
  return ack == ERROR;
}

void(disableDataReporting)() {
  send_cmd_to_mouse(DISABLE_DATA_REPORT);
}

void(enableStreamMode)(){
  send_cmd_to_mouse(SET_STREAM_MODE);
}

void(enableDataReporting)(){
  send_cmd_to_mouse(ENABLE_DATA_REPORT);
}

void(mouse_enable_reporting)(){
  enableStreamMode();
  enableDataReporting();
}
