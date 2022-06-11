#include "auxiliary_data_structures.h"

/**
 * @brief Moves the players without their action
 * 
 * @return 0 if there was no collision, non-zero otherwise
 */
int passive_move_players();

/**
 * @brief moves the plyer according to information received
 * 
 * @param movementInfo information regarding the movement
 * @param isPassiveMovement boolean flag to distinguish passive movement from active movement
 * @return 0 if there was no collision, non-zero otherwise
 */
int move_player(struct MovementInfo movementInfo, bool isPassiveMovement);

/**
 * @brief Starts the game and changes the background color according to the hour of the day 
 * 
 * Draws the players in their initial positions
 * 
 * @param hour the current hour
 * @return 0 upon success, non-zero otherwise
 */
int start_game(uint8_t hour);

/**
 * @brief Gets the color of the pixel in the given position
 * 
 * @param x the position of the pixel in the X axis
 * @param y the position of the pixel in the Y axis
 * @return 0 upon success, non-zero otherwise
 */
int find_color(uint16_t x, uint16_t y);

/**
 * @brief Gets the horizontal resolution of the screen
 * 
 * @return horizontal resolution of the screen
 */
unsigned get_h_res();

/**
 * @brief Gets the vertical resolution of the screen
 * 
 * @return vertical resolution of the screen
 */
unsigned get_v_res();

/**
 * @brief Get the amount of bits of each pixel
 * 
 * @return amount of bits of each pixel
 */
unsigned get_bits_per_pixel();

/**
 * @brief Gets the memory address of the video memory (mapped in the process adress space)
 * 
 * @return the memory address of the video memory
 */
void* get_video_mem();

/**
 * @brief Draws an image in the specified coordinates
 * 
 * @param img image to be drawn
 * @param x the position of the upper left corner of the image in the X axis
 * @param y the position of the upper left corner of the image in the Y axis
 * @return 0 upon success, non-zero otherwise
 */
int draw_img(xpm_image_t img, uint16_t x, uint16_t y);

/**
 * @brief Moves the cursor of the mouse
 * 
 * @param x the displacement relative to the last cursor position, in the X axis
 * @param y the displacement relative to the last cursor position, in the Y axis
 * @param cursor the image of the cursor
 * @return 0 upon success, non-zero otherwise
 */
int mouseMovement(uint16_t x, uint16_t y, xpm_image_t cursor);

/**
 * @brief Checks if the cursor of the mouse is inside a given rectangle
 * 
 * @param x1 X coordinates of upper left corner of the "button"
 * @param y1 Y coordinates of upper left corner of the "button"
 * @param x2 X coordinates of lower right corner of the "button"
 * @param y2 Y coordinates of lower right corner of the "button"
 * @return true the mouse is inside the "button"
 * @return false the mouse is not inside the "button"
 */
bool mouseInPlace(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/**
 * @brief Initializes the position of the cursor of the mouse
 * 
 * @param cursor image of the cursor
 * @return 0 upon success, non-zero otherwise
 */
int setMouseInitPos(xpm_image_t cursor);

/**
 * @brief Loads an image from a xpm image
 * 
 * @param xpm the xpm image to be loaded
 * @return the loaded image
 */
xpm_image_t load_image(xpm_map_t xpm);

/**
 * @brief Draws the mouse's cursor in the given position
 * 
 * @param img the image of the cursor
 * @param x the position of the upper left corner of the cursor in the X axis
 * @param y the position of the upper left corner of the cursor in the Y axis
 * @return 0 upon success, non-zero otherwise
 */
int draw_cursor(xpm_image_t img, uint16_t x, uint16_t y);

