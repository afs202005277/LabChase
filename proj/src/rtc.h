#ifndef __RTC
#define __RTC
#include "stdint.h"

/** @defgroup rtc rtc
 * @{
 *
 * Real time clock functions
 */


#define RTC_ADDRESS_REG 0x70
#define RTC_DATA_REG 0x71

// data registers
#define RTC_HOURS 4

/**
 * @brief Reads the register passed as a argument
 *
 * @param reg - Register to read
 * @param data - Where the content read is returned
 * @return int - Returns 0 if sucessful and 1 if not
 */
int rtc_read_reg(uint8_t reg, uint8_t *data);

/**
 * @brief Read the year
 *
 * @param year - Where the year value is returned
 * @return int - Returns 0 if sucessful and 1 if not
 */

int read_hours(uint8_t *hours);


/** @} */
#endif /** __RTC */
