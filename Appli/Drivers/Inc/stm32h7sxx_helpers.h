/*
 * stm32h7sxx_helpers.h
 *
 *  Created on: 30 Haz 2026
 *      Author: talha
 */

#ifndef INC_STM32H7SXX_HELPERS_H_
#define INC_STM32H7SXX_HELPERS_H_

#include "stm32h7sxx.h"

/*
 * System clock values.
 *
 * Current RCC configuration:
 * SYSCLK = 560 MHz
 * CPUCLK = 280 MHz
 * HCLK   = 140 MHz
 * APB    = 70 MHz
 */
#define STM32H7SXX_CPU_CLOCK_HZ         280000000UL
#define STM32H7SXX_CPU_CLOCK_MHZ        280UL

/*
 * Common system helper functions.
 */
void DWT_Init(void);
uint32_t GetCycleCount(void);
void BlockingDelay_us(uint32_t us);
void BlockingDelay_ms(uint32_t ms);

/*
 * Fault handlers
 */
void CPU_ClearFaultStatus(void);

#endif /* INC_STM32H7SXX_HELPERS_H_ */
