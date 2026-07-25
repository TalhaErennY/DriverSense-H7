/*
 * 003_RTOS_TriLED.c
 *
 *  Created on: 25 Tem 2026
 *      Author: talha
 */
#include "stm32h7sxx.h"
#include "stm32h7sxx_helpers.h"
#include "stm32h7sxx_gpio_driver.h"
#include "stm32h7sxx_rcc_driver.h"

#include "FreeRTOS.h"
#include "task.h"

#include "SEGGER_SYSVIEW.h"

static void LD1Task(void *pvParameters);
static void LD2Task(void *pvParameters);
static void LD3Task(void *pvParameters);

void Test_003_RTOS_TriLED(void){
	RCC_SystemClock_Config_280MHz_HSI();
	DWT_Init();

	TaskHandle_t task1_handle;
	TaskHandle_t task2_handle;
	TaskHandle_t task3_handle;
	BaseType_t status;

	GPIO_Handle_t LD1;
	GPIO_Handle_t LD2;
	GPIO_Handle_t LD3;

	//LD1
	LD1.pGPIOx = GPIOD;
	LD1.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_10;
	LD1.GPIO_PinConfig.GPIO_PinMode   = GPIO_MODE_OUT;
	LD1.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	LD1.GPIO_PinConfig.GPIO_PinPuPdCtrl = GPIO_PUPD_NONE;

	//LD2
	LD2.pGPIOx = GPIOD;
	LD2.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	LD2.GPIO_PinConfig.GPIO_PinMode   = GPIO_MODE_OUT;
	LD2.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	LD2.GPIO_PinConfig.GPIO_PinPuPdCtrl = GPIO_PUPD_NONE;

	//LD3
	LD3.pGPIOx = GPIOB;
	LD3.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	LD3.GPIO_PinConfig.GPIO_PinMode   = GPIO_MODE_OUT;
	LD3.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	LD3.GPIO_PinConfig.GPIO_PinPuPdCtrl = GPIO_PUPD_NONE;

	GPIO_PeriClockControl(GPIOD, ENABLE);
	GPIO_PeriClockControl(GPIOB, ENABLE);

	GPIO_Init(&LD1);
	GPIO_Init(&LD2);
	GPIO_Init(&LD3);

	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();

	status = xTaskCreate(LD1Task, "LD1", 256, NULL, 2, &task1_handle);
	configASSERT(status == pdPASS);

	status = xTaskCreate(LD2Task, "LD2", 256, NULL, 2, &task2_handle);
	configASSERT(status == pdPASS);

	status = xTaskCreate(LD3Task, "LD3", 256, NULL, 2, &task3_handle);
	configASSERT(status == pdPASS);

	vTaskStartScheduler();

	while(1);
}

static void LD1Task(void *pvParameters){
	//1000ms Toggle
	(void)pvParameters;

	while(1){
		GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_10);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

static void LD2Task(void *pvParameters){
	//500ms Toggle
	(void)pvParameters;

	while(1){
		GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_13);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

static void LD3Task(void *pvParameters){
	//100ms Toggle
	(void)pvParameters;

	while(1){
		GPIO_ToggleOutputPin(GPIOB, GPIO_PIN_NO_7);
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

