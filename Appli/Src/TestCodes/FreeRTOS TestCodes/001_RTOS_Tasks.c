/*
 * 001_RTOS_Tasks.c
 *
 *  Created on: 22 Haz 2026
 *      Author: talha
 */

#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

static void task1_handler(void* parameters);
static void task2_handler(void* parameters);

void Test_001_RTOS_Tasks(void){
	TaskHandle_t task1_handle;
	TaskHandle_t task2_handle;

	BaseType_t status;

	status = xTaskCreate(task1_handler, "Task1", 200, "Hello world from task-1", 2, &task1_handle);

	configASSERT(status == pdPASS);

	status = xTaskCreate(task2_handler, "Task2", 200, "Hello world from task-2", 2, &task2_handle);

	configASSERT(status == pdPASS);

	vTaskStartScheduler();

	//if the control comes here then the launch fot he scheduler has failed due to insufficient memory in the heap
	while(1);
}

static void task1_handler(void* parameters){
	while(1){
		printf("%s\n", (char*)parameters);
		vTaskDelay(1000);
	}

}

static void task2_handler(void* parameters){
	while(1){
		printf("%s\n", (char*)parameters);
		vTaskDelay(1000);
	}

}
