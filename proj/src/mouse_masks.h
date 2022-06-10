#ifndef __MOUSE_MASKS
#define __MOUSE_MASKS

/** @defgroup mouse_masks mouse_masks
 * @{
 *
 * Constants for using the mouse.
 */

#define LB BIT(0) /**< @brief Bit mask to check if the left button is being pressed*/
#define RB BIT(1) /**< @brief Bit mask to check if the right button is being pressed*/
#define MB BIT(2) /**< @brief Bit mask to check if the middle button is being pressed*/

#define X_OV BIT(6) /**< @brief Bit mask to check if there was overflow in the X axis*/
#define Y_OV BIT(7) /**< @brief Bit mask to check if there was overflow in the Y axis*/

#define X_SIGNAL BIT(4) /**< @brief Bit mask to check if the displacement in the X axis is positive or negative*/
#define Y_SIGNAL BIT(5) /**< @brief Bit mask to check if the displacement in the Y axis is positive or negative */

/**@}*/
#endif /* __MOUSE_MASKS */
