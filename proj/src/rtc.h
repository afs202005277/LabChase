#ifndef __RTC
#define __RTC
#include "stdint.h"

/** @defgroup rtc rtc
 * @{
 *
 * Real time clock functions
 */

#define RTC_IRQ 8

#define RTC_ADDRESS_REG 0x70
#define RTC_DATA_REG 0x71

// data registers
#define RTC_HOURS 4
#define RTC_MINUTES 2

/**
 * @brief Subscribes interrupts from the rtc
 *
 * @param irq_bitmask - id of the bitmask
 * @return int - Returns 0 if sucessful and 1 if not
 */
int rtc_subscribe_int(uint8_t *irq_bitmask);

/**
 * @brief Unsbscribes the rtc interrupts
 *
 * @return int - Returns 0 if sucessful and 1 if not
 */
int rtc_unsubscribe_int();

/**
 * @brief Reads the register passed as a argument
 *
 * @param reg - Register to read
 * @param data - Where the content read is returned
 * @return int - Returns 0 if sucessful and 1 if not
 */
int rtc_read_reg(uint8_t reg, uint8_t *data);

/**
 * @brief
 *
 * @param reg - Register to write to
 * @param data - The data to be written
 * @return int - Returns 0 if sucessful and 1 if not
 */
int rtc_write_reg(uint8_t reg, uint8_t data);

/**
 * @brief Read the year
 *
 * @param year - Where the year value is returned
 * @return int - Returns 0 if sucessful and 1 if not
 */

int read_hours(uint8_t *hours);

/**
 * @brief Reads the minutes
 *
 * @param minutes - Where the minutes value is returned
 * @return int - Returns 0 if sucessful and 1 if not
 */
int read_minutes(uint8_t *minutes);

/** @} */
#endif /** __RTC */
