#ifndef __KBC_MACROS
#define __KBC_MACROS

/** @defgroup KBC_macros KBC_macros
 * @{
 *
 * Constants for programming the keyboard controller.
 */

#define READ_CMD_BYTE 0x20  /**< @brief Instruction to read the KBC's command byte */
#define WRITE_CMD_BYTE 0x60 /**< @brief Instruction to write the new command byte to the KBC*/
#define WRITE_TO_MOUSE 0xD4 /**< @brief Instruction to redirect bytes received by the KBC to the mouse */

#define KBC_OUT_BUF 0x60 /**< @brief Memory address of the KBC's output buffer */
#define KBC_ST_REG 0x64  /**< @brief Memory address of the KBC's status register */
#define KBC_CMD_REG 0x64 /**< @brief Memory address of the KBC's command register */
#define KBC_IN_BUF 0x60  /**< @brief Mmemory address of the KBC's input buffer */

#define KBC_PAR_ERR BIT(7)  /**< @brief Bit mask to check if there was a parity error */
#define KBC_ST_IBF BIT(1)   /**< @brief Bit mask to check if the input buffer is full (if set, can't write to the input buffer or to the command register) */
#define KBC_OUT_FULL BIT(0) /**< @brief Bit mask to check if the output buffer is full (if set, there is data available for reading) */
#define KBC_TO_ERR BIT(6)   /**< @brief Bit mask to check if there was a timeout error */
#define KBC_AUX BIT(5)      /**< @brief Bit mask to check if the input is from the mouse or the keyboard (if set, the input is from the mouse) */

#define MOUSE_IRQ 12        /**< @brief Mouse IRQ line */
#define KBD_IRQ 1           /**< @brief Keyboard IRQ line */
#define KBD_ERROR_BYTE 0xFF /**< @brief Byte used to signal an error when reading from the keyboard */
#define MOUSE_DELAY 20000   /**< @brief Delay used between consecutive tries when sending bytes to the mouse */
#define KEYBOARD_DELAY 5000 /**< @brief Delay used between consecutive tries when sending bytes to the keyboard */

#define DISABLE_DATA_REPORT 0xF5 /**< @brief Command used to disable data reporting (mouse) */
#define ENABLE_DATA_REPORT 0xF4  /**< @brief Command used to enable data reporting (mouse) */
#define SET_STREAM_MODE 0xEA     /**< @brief Command used to activate mouse's stream mode */

#define ACK 0xFA   /**< @brief Byte sent by the mouse to signal a successful receive */
#define NACK 0xFE  /**< @brief Byte sent by the mouse to signal the need of the entire command to be sent again */
#define ERROR 0xFC /**< @brief Byte sent by the mouse to signal an error */

/**@}*/
#endif /* __KBC_MACROS */
