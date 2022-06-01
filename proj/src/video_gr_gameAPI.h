#include "auxiliary_data_structures.h"


int passive_move_players();

int move_player(struct MovementInfo movementInfo, bool isPassiveMovement);

int start_game(uint16_t mode, uint8_t hour);

int (find_color)(uint16_t x, uint16_t y);

int (xpm_drawer)(xpm_map_t xpm, uint16_t x, uint16_t y);
