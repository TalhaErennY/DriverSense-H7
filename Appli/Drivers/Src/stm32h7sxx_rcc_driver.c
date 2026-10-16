/*
 * stm32h7sxx_rcc_driver.c
 *
 *  Created on: 22 Haz 2026
 *      Author: talha
 */
#include <stdint.h>
#include "stm32h7sxx.h"
#include "stm32h7sxx_rcc_driver.h"
#include "stm32h7sxx_flash_driver.h"

//Delay function
static void RCC_Delay(volatile uint32_t delay){
	while(delay--){
		__asm volatile ("nop");
	}
}

/*
 * Important note for FreeRTOS:
 * configCPU_CLOCK_HZ must be 280000000UL for this configuration,
 * because the Cortex-M7 CPU clock is SYSCLK / CPRE = 560 MHz / 2.
 */
void RCC_SystemClock_Config_280MHz_HSI(void){
    /*
     * 0. Before increasing CPU/AXI clocks, increase Flash wait states.
     *
     * According to FLASH recommended wait-state table:
     *
     * AXI interface clock = 140 MHz
     * VOS low range
     *
     * LATENCY    = 0x3
     * WRHIGHFREQ = 0b01
     */
    //FLASH_SetLatency(FLASH_ACR_LATENCY_3WS);
	//FLASH_SetLatencyto3();
    //FLASH_SetWriteHighFrequency(FLASH_ACR_WRHIGHFREQ_01);

    /*
     * Target:
     *
     * HSI = 64 MHz
     * DIVM1 = /4       -> FREF = 16 MHz
     * DIVN1 = 34       -> x35, PLLCLK = 560 MHz
     * DIVP1 = 0        -> /1, PLL1_P = 560 MHz
     *
     * SYSCLK = PLL1_P  -> 560 MHz
     * CPRE = /2        -> CPUCLK = 280 MHz
     * BMPRE = /1       -> HCLK = 280 MHz
     * PPRE1/2/4/5 = /2 -> APB = 140 MHz
     */


    /*
     * 1. Enable HSI and wait ready.
     */
	RCC->CR |= (1UL << RCC_CR_HSION);

	while((RCC->CR & (1UL << RCC_CR_HSIRDY)) == 0U){
		// Wait until HSI ready
	}

    /*
     * 2. Temporarily switch SYSCLK to HSI.
     */
	RCC->CFGR &= ~(0x7UL << RCC_CFGR_SW);
	RCC->CFGR |= (RCC_CFGR_SW_HSI << RCC_CFGR_SW);

	while(((RCC->CFGR >> RCC_CFGR_SWS) & 0x7UL) != RCC_CFGR_SW_HSI){
		// Wait until HSI clock is valid
	}

    /*
     * 3. Disable PLL1 outputs.
     */
	RCC->PLLCFGR &= ~((1UL << RCC_PLLCFGR_PLL1PEN) |
					  (1UL << RCC_PLLCFGR_PLL1QEN) |
					  (1UL << RCC_PLLCFGR_PLL1SEN));

    /*
     * 4. Disable PLL1 and wait PLL1RDY = 0.
     */
	RCC->CR &= ~(1UL << RCC_CR_PLL1ON);

	while((RCC->CR & (1UL << RCC_CR_PLL1RDY)) != 0U){
		//Wait until PLL1 is disabled
	}

    /*
     * 5. Select PLL source HSI and DIVM1 = 4.
     */
	RCC->PLLCKSELR &= ~((0x3UL << RCC_PLLCKSELR_PLLSRC) | (0xFFUL << RCC_PLLCKSELR_DIVM1));
	RCC->PLLCKSELR |= ((RCC_PLLCKSELR_PLLSRC_HSI << RCC_PLLCKSELR_PLLSRC) | (4UL << RCC_PLLCKSELR_DIVM1));

    /*
     * 6. Clear fractional mode for integer PLL.
     */
	RCC->PLLCFGR &= ~(1UL << RCC_PLLCFGR_PLL1FRACEN);
	RCC->PLL1FRACR = 0U;
	RCC->PLLCFGR |=  (1UL << RCC_PLLCFGR_PLL1FRACEN);
	RCC->PLL1FRACR = 0U;

	RCC_Delay(2000U);

    /*
     * 7. Configure PLL1 RGE, VCOSEL, SSCGEN.
     */
	RCC->PLLCFGR &= ~((0x3UL << RCC_PLLCFGR_PLL1RGE) 	|
					  (0x1UL << RCC_PLLCFGR_PLL1VCOSEL) |
					  (0x1UL << RCC_PLLCFGR_PLL1SSCGEN));

	RCC->PLLCFGR |= (RCC_PLLCFGR_PLLxRGE_8to16M << RCC_PLLCFGR_PLL1RGE);
	RCC->PLLCFGR |= (RCC_PLLCFGR_PLLxVCOSEL_VCOH << RCC_PLLCFGR_PLL1VCOSEL);

    /*
     * 8. Configure PLL1 DIVN/DIVP/DIVQ.
     */
	RCC->PLL1DIVR1 = 0U;
	RCC->PLL1DIVR1 |= ((34UL << RCC_PLL1DIVR1_DIVN) | (0UL << RCC_PLL1DIVR1_DIVP) | (0UL << RCC_PLL1DIVR1_DIVQ)); //DIVN: 34 (for the divider it is 35), DIVP: 1, DIVQ: not used

    /*
     * 9. Configure domain prescalers.
     */
		/*
		 * CPRE = /2
		 */
		RCC->CDCFGR &= ~(0xFUL << RCC_CDCFGR_CPRE);
		RCC->CDCFGR |= (RCC_CDCFGR_CPRE_DIV2 << RCC_CDCFGR_CPRE);

		/*
		 * BMPRE = /1
		 */
		RCC->BMCFGR &= ~(0xFUL << RCC_BMCFGR_BMPRE);
		RCC->BMCFGR |= (RCC_BMCFGR_BMPRE_DIV2 << RCC_BMCFGR_BMPRE);

		/*
		 * PPRE1/2/4/5 = /2
		 */
		RCC->APBCFGR &= ~((0x7UL << RCC_APBCFGR_PPRE1) |
						  (0x7UL << RCC_APBCFGR_PPRE2) |
						  (0x7UL << RCC_APBCFGR_PPRE4) |
						  (0x7UL << RCC_APBCFGR_PPRE5));

		RCC->APBCFGR |= ((RCC_APBCFGR_PPREx_DIV2 << RCC_APBCFGR_PPRE1) |
						 (RCC_APBCFGR_PPREx_DIV2 << RCC_APBCFGR_PPRE2) |
						 (RCC_APBCFGR_PPREx_DIV2 << RCC_APBCFGR_PPRE4) |
						 (RCC_APBCFGR_PPREx_DIV2 << RCC_APBCFGR_PPRE5));

    /*
     * 10. Enable PLL1 and wait ready.
     */
	RCC->CR |= (1UL << RCC_CR_PLL1ON);

	while((RCC->CR & (1UL << RCC_CR_PLL1RDY)) == 0U){
		//Wait until PLL1 Ready
	}

    /*
     * 11. Enable PLL1 P output.
     */
	RCC->PLLCFGR |= (1UL << RCC_PLLCFGR_PLL1PEN);


    /*
     * 12. Switch SYSCLK source to PLL1.
     */
	RCC->CFGR &= ~(0x7UL << RCC_CFGR_SW);
	RCC->CFGR |= (RCC_CFGR_SW_PLL1 << RCC_CFGR_SW);

	while(((RCC->CFGR >> RCC_CFGR_SWS) & 0x7UL) != RCC_CFGR_SW_PLL1){
		//Wait until PLL1 selected as SYSCLK source
	}

}

/****************************************************************************
 * @fn				- RCC_SDMMC_ClockConfig_50MHz
 *
 * @brief			- This function configures PLL2_S as 50 MHz
 *					  kernel clock source for SDMMC1/SDMMC2
 *
 * @param[in]		- none
 *
 * @return			- none
 *
 * @Note			- HSI = 64 MHz
 *					  DIVM2 register = 8  -> /8
 *					  DIVN2 register = 49 -> x50
 *					  DIVS2 register = 7  -> /8
 *
 */
void RCC_SDMMC_ClockConfig_50MHz(void){

	/*
	 * PLL2_S must be disabled before changing DIVS2.
	 */
	RCC->PLLCFGR &= ~(1UL << RCC_PLLCFGR_PLL2SEN);


	/*
	 * Disable PLL2 before configuration.
	 */
	RCC->CR &= ~(1UL << RCC_CR_PLL2ON);

	while((RCC->CR & (1UL << RCC_CR_PLL2RDY)) != 0U){
		// Wait until PLL2 is disabled
	}


	/*
	 * HSI = 64 MHz
	 *
	 * DIVM2 = 8
	 *
	 * PLL2 reference clock:
	 *
	 * 64 MHz / 8 = 8 MHz
	 */
	RCC->PLLCKSELR &=
			~(0x3FUL << RCC_PLLCKSELR_DIVM2);

	RCC->PLLCKSELR |=
			(8UL << RCC_PLLCKSELR_DIVM2);


	/*
	 * PLL2 reference clock = 8 MHz
	 *
	 * Select 8 - 16 MHz PLL input range.
	 */
	RCC->PLLCFGR &=
			~(0x3UL << RCC_PLLCFGR_PLL2RGE);

	RCC->PLLCFGR |=
			(RCC_PLLCFGR_PLLxRGE_8to16M
			<< RCC_PLLCFGR_PLL2RGE);


	/*
	 * Select VCOH.
	 */
	RCC->PLLCFGR &=
			~(1UL << RCC_PLLCFGR_PLL2VCOSEL);


	/*
	 * Disable fractional PLL.
	 */
	RCC->PLLCFGR &=
			~(1UL << RCC_PLLCFGR_PLL2FRACEN);

	RCC->PLL2FRACR = 0U;


	/*
	 * DIVN2 register = 49
	 *
	 * Actual multiplication factor:
	 * DIVN2 + 1 = 50
	 *
	 * 8 MHz * 50 = 400 MHz VCO
	 */
	RCC->PLL2DIVR1 &=
	        ~(0x1FFUL << RCC_PLL2DIVR1_DIVN);

	RCC->PLL2DIVR1 |=
	        (49UL << RCC_PLL2DIVR1_DIVN);


	/*
	 * DIVS2 register = 7
	 *
	 * Actual division factor:
	 * DIVS2 + 1 = 8
	 *
	 * 400 MHz / 8 = 50 MHz
	 */
	RCC->PLL2DIVR2 &=
	        ~(0x7UL << RCC_PLL2DIVR2_DIVS);

	RCC->PLL2DIVR2 |=
	        (7UL << RCC_PLL2DIVR2_DIVS);


	/*
	 * Enable PLL2_S output.
	 */
	RCC->PLLCFGR |=
			(1UL << RCC_PLLCFGR_PLL2SEN);


	/*
	 * Enable PLL2.
	 */
	RCC->CR |=
			(1UL << RCC_CR_PLL2ON);

	while((RCC->CR &
			(1UL << RCC_CR_PLL2RDY)) == 0U){
		// Wait until PLL2 is ready
	}


	/*
	 * SDMMC12SEL = 0
	 *
	 * SDMMC kernel clock = pll2_s_ck.
	 */
	RCC->CCIPR1 &=
			~(1UL << RCC_CCIPR1_SDMMC12SEL);
}
