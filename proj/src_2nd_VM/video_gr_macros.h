#ifndef _VIDEO_GR_MACROS
#define _VIDEO_GR_MACROS

/** @defgroup video_graphics_macros video_graphics_macros
 * @{
 *
 * Constants for using the video graphics
 */

#define MOVEMENT_STEP 5;          /**< @brief Displacement of each movement */
#define SIZE_FRONT_END 5;         /**< @brief Size of the player's line */
#define COLOR_PLAYER2 0x1F51FF;   /**< @brief Color of player 1 */
#define COLOR_PLAYER1 0xFF5F1F;   /**< @brief Color of player 2 */
#define MOUSE_BORDER_WIDTH 30     /**< @brief Thickness of the border that the mouse cannot breach (around the entire screen) */
#define VBE_SERVICES 0x4F;        /**< @brief The code of the VBE services */
#define SET_VBE_MOD 0x02          /**< @brief Command to set the mode of the VBE */
#define ENABLE_LIN_BUF BIT(14)    /**< @brief Flag to enable the linear frame buffer */
#define BIOS_VIDEO_SERVICES 0x10  /**< @brief Code associated with the BIOS video services */
#define NIGHT_BACKGROUND 0x000000 /**< @brief Background color in night mode */
#define DAY_BACKGROUND 0xFFFFFF   /**< @brief Background color in day mode */
#define TRANSPARENCY 0x123456     /**< @brief Color that will be interpreted as transparent */

/**@}*/
#endif /* _VIDEO_GR_MACROS */
