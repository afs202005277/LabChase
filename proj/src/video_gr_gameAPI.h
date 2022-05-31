#include "auxiliary_data_structures.h"

int passive_move_players();

int move_player(struct MovementInfo movementInfo, bool isPassiveMovement);

int start_game();

int (find_color)(uint16_t x, uint16_t y);

int (xpm_drawer)(xpm_map_t xpm);

int new_vg_init(uint16_t mode);

unsigned get_h_res();

unsigned get_v_res();

unsigned get_bits_per_pixel();

void* get_video_mem();
