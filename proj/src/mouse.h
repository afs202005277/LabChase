#ifndef __MOUSE_H
#define __MOUSE_H
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>
#include <util.h>

int(mouse_subscribe_int)(uint8_t *bit_no);

int(mouse_unsubscribe_int)();

void(mouse_ih)();

void(parse_mouse_bytes)(struct packet* pp);

void(disableDataReporting)();

#endif /* __KEYBOARD_H */
