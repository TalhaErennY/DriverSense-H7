/*
 * 002_RTOS_Led.c
 *
 *  Created on: 30 Haz 2026
 *      Author: talha
 */
#include "stm32h7sxx.h"
#include "stm32h7sxx_helpers.h"
#include "stm32h7sxx_gpio_driver.h"
#include "stm32h7sxx_rcc_driver.h"

#include "FreeRTOS.h"
#include "task.h"

#include "SEGGER_SYSVIEW.h"

static void LedTask(void *pvParameters);

void Test_002_RTOS_Led(void){
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

	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();

	xTaskCreate(LedTask, "LED", 256, NULL, 1, NULL);

	vTaskStartScheduler();

	while(1){
	}
}

static void LedTask(void *pvParameters){
	(void)pvParameters;

	while(1){
		GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_13);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
