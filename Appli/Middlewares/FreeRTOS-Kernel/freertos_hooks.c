/*
 * freertos_hooks.c
 *
 *  Created on: 29 Haz 2026
 *      Author: talha
 */

#include "FreeRTOS.h"
#include "task.h"

volatile TaskHandle_t g_stack_overflow_task = 0;
volatile char *g_stack_overflow_task_name = 0;

void vApplicationMallocFailedHook(void){
	while(1);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName){
	g_stack_overflow_task = xTask;
	g_stack_overflow_task_name = pcTaskName;

	while(1);
}

