/*
 * stm32h7sxx_helpers.c
 *
 *  Created on: 30 Haz 2026
 *      Author: talha
 */

#include "stm32h7sxx.h"
#include "stm32h7sxx_helpers.h"

//Fault Handlers
void CPU_ClearFaultStatus(void){
    /*
     * CFSR: write 1 to clear fault bits
     * HFSR: write 1 to clear fault bits
     * DFSR: write 1 to clear debug fault bits
     */
    *((volatile uint32_t *)0xE000ED28UL) = 0xFFFFFFFFUL; /* CFSR */
    *((volatile uint32_t *)0xE000ED2CUL) = 0xFFFFFFFFUL; /* HFSR */
    *((volatile uint32_t *)0xE000ED30UL) = 0xFFFFFFFFUL; /* DFSR */
}


/*
 * Enables DWT cycle counter.
 *
 * DWT_CYCCNT counts CPU cycles. With CPUCLK = 280 MHz:
 *
 * Example:
 * 1 us = 280 cycles
 * 1 ms = 280000 cycles
 */
void DWT_Init(void){
	/*
	 * Enable trace and debug block
	 */
	COREDEBUG_DEMCR |= (1UL << COREDEBUG_DEMCR_TRCENA);

	/*
	 * Clear cycle counter
	 */
	DWT_CYCCNT = 0U;

	/*
	 * Enable DWT Cycle counter
	 */
	DWT_CTRL |= (1UL << DWT_CTRL_CYCCNTENA);
}

/*
 * Returns current CPU cycle counter value.
 */
uint32_t GetCycleCount(void)
{
    return DWT_CYCCNT;
}

/*
 * Delay in microseconds using DWT cycle counter.
 *
 * This delay depends on STM32H7SXX_CPU_CLOCK_HZ being equal to
 * the real CPU clock.
 */
void BlockingDelay_us(uint32_t us){
	uint32_t start;
	uint32_t cycles;

	start = DWT_CYCCNT;
	cycles = us * STM32H7SXX_CPU_CLOCK_MHZ;

	while((DWT_CYCCNT - start) < cycles){

	}
}

/*
 * Delay in milliseconds using DWT cycle counter.
 */
void BlockingDelay_ms(uint32_t ms){
    while(ms > 0U){
        BlockingDelay_us(1000U);
        ms--;
    }
}
