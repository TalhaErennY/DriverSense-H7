/*
 * 013_RCC_ClkConf_Test.c
 *
 *  Created on: 29 Haz 2026
 *      Author: talha
 */

#include "stm32h7sxx.h"
#include "stm32h7sxx_helpers.h"
#include "stm32h7sxx_gpio_driver.h"
#include "stm32h7sxx_rcc_driver.h"

void Test_013_RCC_ClockConfiguration(void){
	RCC_SystemClock_Config_280MHz_HSI();

	DWT_Init();

	GPIO_Handle_t GPIOLed;

	//USER LED2 (Yellow) connected to PD13
	GPIOLed.pGPIOx = GPIOD;
	GPIOLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIOLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GPIOLed.GPIO_PinConfig.GPIO_PinPuPdCtrl = GPIO_PUPD_NONE;
	GPIOLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;

	GPIO_PeriClockControl(GPIOD, ENABLE);

	GPIO_Init(&GPIOLed);

	while(1){
		GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_13);
		BlockingDelay_ms(1000U);
	}
}
