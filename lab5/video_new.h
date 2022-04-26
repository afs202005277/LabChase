#pragma once
#include <stdint.h>
unsigned get_h_res();

unsigned get_v_res();

unsigned get_bits_per_pixel();

uint8_t get_red_mask_size();

uint8_t get_blue_mask_size();

uint8_t get_green_mask_size();

int xpm_drawer(xpm_map_t xpm, uint16_t x, uint16_t y);
