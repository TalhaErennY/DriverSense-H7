/*
 * 001Led_Toggle.c
 *
 *  Created on: May 23, 2026
 *      Author: talha
 */

#include "TestCodes.h"
#include "stm32h7sxx.h"
#include "stm32h7sxx_gpio_driver.h"

static void delay(uint32_t clkcycle){
    for(volatile uint32_t i = 0; i < clkcycle; i++);
}


void Test_001_Led_Toggle(void){
	GPIO_Handle_t GPIOLed;

	GPIOLed.pGPIOx = GPIOB;
	GPIOLed.GPIO_PinConfig.GPIO_PinNumber   = GPIO_PIN_NO_10;
	GPIOLed.GPIO_PinConfig.GPIO_PinMode	    = GPIO_MODE_OUT;
	GPIOLed.GPIO_PinConfig.GPIO_PinPuPdCtrl = GPIO_PUPD_PU;
	GPIOLed.GPIO_PinConfig.GPIO_PinOPType   = GPIO_OP_TYPE_OD;

	GPIO_PeriClockControl(GPIOB, ENABLE);

	GPIO_Init(&GPIOLed);

	while(1){
		GPIO_ToggleOutputPin(GPIOB, GPIO_PIN_NO_10);
		delay(10000000);
	}
}

