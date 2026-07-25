/*
 * stm32h7sxx_flash_driver.c
 *
 *  Created on: 29 Haz 2026
 *      Author: talha
 */

#include "stm32h7sxx_flash_driver.h"

void FLASH_SetLatency(uint32_t latency){
	/*
	 * Clear LATENCY Field
	 */
	FLASH_REG->ACR &= ~(FLASH_ACR_LATENCY_MSK);

	/*
	 * New latency value
	 */
	FLASH_REG->ACR |= (latency << FLASH_ACR_LATENCY);

	/*
	 * Read back until written value is active
	 */
	while((FLASH_REG->ACR & FLASH_ACR_LATENCY_MSK) != (latency << FLASH_ACR_LATENCY)){
		//Wait
	}
}

void FLASH_SetLatencyto3(void){
	/*
	 * Clear LATENCY Field
	 */
	FLASH_REG->ACR &= ~(FLASH_ACR_LATENCY_MSK);

	/*
	 * New latency value
	 */
	FLASH_REG->ACR |= (3U << FLASH_ACR_LATENCY);

	/*
	 * Read back until written value is active
	 */
	while((FLASH_REG->ACR & FLASH_ACR_LATENCY_MSK) != (3U << FLASH_ACR_LATENCY)){
		//Wait
	}
}

void FLASH_SetWriteHighFrequency(uint32_t wr_high_freq){
	FLASH_REG->ACR &= ~(FLASH_ACR_WRHIGHFREQ_MSK);
	FLASH_REG->ACR |= (wr_high_freq << FLASH_ACR_WRHIGHFREQ);

	while((FLASH_REG->ACR &= FLASH_ACR_WRHIGHFREQ_MSK) != (wr_high_freq << FLASH_ACR_WRHIGHFREQ)){
		//Wait
	}
}
