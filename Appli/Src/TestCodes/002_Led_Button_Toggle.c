/*
 * 002Led_Button_Toggle.c
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


void Test_002_Led_Button_Toggle(void){
	GPIO_Handle_t GPIOLed;
	GPIO_Handle_t GPIOButton;

	//Button Connected to PB11
	GPIOButton.pGPIOx = GPIOB;
	GPIOButton.GPIO_PinConfig.GPIO_PinNumber   = GPIO_PIN_NO_11;
	GPIOButton.GPIO_PinConfig.GPIO_PinMode     = GPIO_MODE_IN;
	GPIOButton.GPIO_PinConfig.GPIO_PinPuPdCtrl = GPIO_PUPD_PD;


	//LED Connected to PB10
	GPIOLed.pGPIOx = GPIOB;
	GPIOLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_10;
	GPIOLed.GPIO_PinConfig.GPIO_PinMode	  = GPIO_MODE_OUT;
	GPIOLed.GPIO_PinConfig.GPIO_PinPuPdCtrl = GPIO_PUPD_NONE;
	GPIOLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;

	GPIO_PeriClockControl(GPIOB, ENABLE);

	GPIO_Init(&GPIOLed);
	GPIO_Init(&GPIOButton);

	uint8_t flag = 0;

	while(1)
	{
	    if(((GPIO_ReadFromInputPin(GPIOB, GPIO_PIN_NO_11) == 0) && (flag == 0))){
	        // butona basıldı
	    	flag = 1;
	        GPIO_ToggleOutputPin(GPIOB, GPIO_PIN_NO_10);
	    	delay(5000);
	    }
	    else if(((GPIO_ReadFromInputPin(GPIOB, GPIO_PIN_NO_11) == 1) && (flag == 1))){
	        // butona basılmadı
	    	flag = 0;
	        delay(5000);
	    }
	}

}
