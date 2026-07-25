/*
 * stm32h7sxx_flash_driver.h
 *
 *  Created on: 29 Haz 2026
 *      Author: talha
 */

#ifndef INC_STM32H7SXX_FLASH_DRIVER_H_
#define INC_STM32H7SXX_FLASH_DRIVER_H_

#include "stm32h7sxx.h"

/*
 * Flash latency configuration.
 */
void FLASH_SetLatency(uint32_t latency);
void FLASH_SetWriteHighFrequency(uint32_t wr_high_freq);

void FLASH_SetLatencyto3(void);

#endif /* INC_STM32H7SXX_FLASH_DRIVER_H_ */
