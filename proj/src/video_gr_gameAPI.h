#include "auxiliary_data_structures.h"

int passive_move_players();

int move_player(struct MovementInfo movementInfo, bool isPassiveMovement);

int start_game(uint16_t mode);

int (find_color)(uint16_t x, uint16_t y);

int setMouseInitPos ();

int mouseMovement (uint16_t x, uint16_t y);

int xpmDrawer(xpm_map_t xpm, uint16_t x, uint16_t y);

int xpm_drawer(xpm_map_t xpm, uint16_t x, uint16_t y);

xpm_image_t draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y);

bool mouseInPlace(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
