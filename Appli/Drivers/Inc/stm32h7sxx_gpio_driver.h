/*
 * stm32h7sxx_gpio_driver.h
 *
 *  Created on: May 21, 2026
 *      Author: talha
 */

#ifndef INC_STM32H7SXX_GPIO_DRIVER_H_
#define INC_STM32H7SXX_GPIO_DRIVER_H_

#include "stm32h7sxx.h"

typedef struct{
	uint8_t GPIO_PinNumber;				/*!<Possible Values from @GPIO_PIN_NUMBERS>*/
	uint8_t GPIO_PinMode;				/*!<Possible Values from @GPIO_PIN_MODE>*/
	uint8_t GPIO_PinSpeed;				/*!<Possible Values from @GPIO_OUTPUT_SPEEDS>*/
	uint8_t GPIO_PinPuPdCtrl;
	uint8_t GPIO_PinOPType;
	uint8_t GPIO_PinAltFuncMode;
}GPIO_PinConfig_t;

typedef struct{
	GPIO_RegDef_t *pGPIOx;	//This holds the base address of the GPIO Port to which the pin belongs
	GPIO_PinConfig_t GPIO_PinConfig; //Hold GPIO Pin configuration settings
}GPIO_Handle_t;

/*
 * @GPIO_PIN_NUMBERS
 * GPIO Pin Numbers
 */
#define GPIO_PIN_NO_0			0U
#define GPIO_PIN_NO_1			1U
#define GPIO_PIN_NO_2			2U
#define GPIO_PIN_NO_3			3U
#define GPIO_PIN_NO_4			4U
#define GPIO_PIN_NO_5			5U
#define GPIO_PIN_NO_6			6U
#define GPIO_PIN_NO_7			7U
#define GPIO_PIN_NO_8			8U
#define GPIO_PIN_NO_9			9U
#define GPIO_PIN_NO_10			10U
#define GPIO_PIN_NO_11			11U
#define GPIO_PIN_NO_12			12U
#define GPIO_PIN_NO_13			13U
#define GPIO_PIN_NO_14			14U
#define GPIO_PIN_NO_15			15U

/*
 * @GPIO_PIN_MODES
 * GPIO Pin Modes
 */

#define GPIO_MODE_IN			0U	//Input Mode
#define	GPIO_MODE_OUT			1U	//General Purpose Output Mode
#define GPIO_MODE_AF			2U	//Alternate Function Mode
#define GPIO_MODE_ANALOG		3U	//Analog Mode (Reset State)

//Interrupt modes
#define GPIO_MODE_IT_FT			4U  //Falling Edge Trigger
#define GPIO_MODE_IT_RT			5U  //Raising Edge Trigger
#define GPIO_MODE_IT_RFT		6U  //Falling and Raising Edge Trigger

/*
 * GPIO Pin Output Types
 */
#define GPIO_OP_TYPE_PP			0U  //Output Push-Pull (Reset State)
#define GPIO_OP_TYPE_OD			1U	//Output Open-Drain

/*
 * @GPIO_OUTPUT_SPEEDS
 * GPIO Output Speeds
 */
#define GPIO_OSPEED_LOW			0U	//Low Speed
#define GPIO_OSPEED_MEDIUM		1U	//Medium Speed
#define GPIO_OSPEED_HIGH		2U	//High Speed
#define GPIO_OSPEED_VHIGH		3U	//Very High Speed

/*
 * GPIO Pull Up-Down Configurations
 */
#define GPIO_PUPD_NONE			0U 	//No pull-up, pull-down
#define GPIO_PUPD_PU			1U	//Pull-up
#define GPIO_PUPD_PD			2U	//Pull-down


/******************************************************************************************************
 * 								APIs Supported by This Driver
 *       For more information about the APIs check the function definitions
 ******************************************************************************************************/
/*
 * Peripheral Clock Setup
 */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t ENorDI);

/*
 * GPIO Init - DeInıt
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);  //to reset with AHB4RSTR

/*
 * Data Read - Write
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);

/*
 * IRQ Configuration and ISR Handling
 */
void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t ENorDI);		//how will be the irq trigger
void GPIO_IRQHandling(uint8_t PinNumber);	//what happens if irq triggered


#endif /* INC_STM32H7SXX_GPIO_DRIVER_H_ */

