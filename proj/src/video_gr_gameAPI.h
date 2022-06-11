#include "auxiliary_data_structures.h"


int passive_move_players();

int move_player(struct MovementInfo movementInfo, bool isPassiveMovement);

int start_game(uint8_t hour);

int find_color(uint16_t x, uint16_t y);

unsigned get_h_res();

unsigned get_v_res();

unsigned get_bits_per_pixel();

void* get_video_mem();

int draw_img(xpm_image_t img, uint16_t x, uint16_t y);

int mouseMovement(uint16_t x, uint16_t y, xpm_image_t cursor);

bool mouseInPlace(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

int setMouseInitPos(xpm_image_t cursor);

xpm_image_t load_image(xpm_map_t xpm);

int draw_cursor(xpm_image_t img, uint16_t x, uint16_t y);

