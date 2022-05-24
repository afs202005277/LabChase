#pragma once
#include <stdint.h>
#include <stdio.h>
#include <lcom/xpm.h>

unsigned get_h_res();

unsigned get_v_res();

unsigned get_bits_per_pixel();

uint8_t get_red_mask_size();

uint8_t get_blue_mask_size();

uint8_t get_green_mask_size();

int xpm_drawer(xpm_map_t xpm, uint16_t x, uint16_t y);

void* new_vg_init(uint16_t mode);

int xpm_move(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate);

int start_screen(uint16_t x1, uint16_t y1, uint32_t color1, uint16_t x2, uint16_t y2, uint32_t color2, uint16_t OBJ_SIZE);
