/*
 * stm32h7sxx_gpio_driver.c
 *
 *  Created on: May 21, 2026
 *      Author: talha
 */

#include "stm32h7sxx_gpio_driver.h"
/*
 * Peripheral Clock Control
 */

/****************************************************************************
 * @fn				- GPIO_PeriClockControl
 *
 * @brief			- This function enables or disables peripheral clock for given GPIO Port
 *
 * @param[in]		- base address of the GPIO Peripheral
 * @param[in]		- ENABLE or DISABLE macros
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- none
 *
 */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t ENorDI)
{
	uintptr_t gpio_addr = (uintptr_t)pGPIOx;
	uint32_t port_index;

	if(((gpio_addr - GPIOA_BASEADDR) % 0x400UL) != 0U)
	{
	    return;
	}

	if((gpio_addr >= GPIOA_BASEADDR) && (gpio_addr <= GPIOH_BASEADDR)){
		port_index = (gpio_addr - GPIOA_BASEADDR) / 0x400UL;
	} else if ((gpio_addr >= GPIOM_BASEADDR) && (gpio_addr <= GPIOP_BASEADDR)){
		port_index = (gpio_addr - GPIOA_BASEADDR) / 0x400UL;
	} else{
		//Error invalid port address
		return;
	}

	if(ENorDI == ENABLE){
		RCC->AHB4ENR |= (1UL << port_index);
	} else if(ENorDI == DISABLE){
		RCC->AHB4ENR &= ~(1UL << port_index);
	} else {
		//Only enable or disable can be written
		return;
	}
}

/*
 * GPIO Init - DeInıt
 */

/****************************************************************************
 * @fn				- GPIO_Init
 *
 * @brief			- This function initializes the GPIO Port
 *
 * @param[in]		- GPIO handler configuration
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- none
 *
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle){
	//Temp register
	uint32_t temp = 0;
	uint32_t pin = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber;

	//1. Configure the mode of GPIO Pin
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG){
		//non-interrupt modes
		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << ( 2U * pin));
		pGPIOHandle->pGPIOx->MODER &= ~(3U << (2U * pin));
		pGPIOHandle->pGPIOx->MODER |= temp;
	}else{
		//interrupts modes (later)
	}
	temp = 0;

	//2. Configure the Output Speed
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << ( 2U * pin));
	pGPIOHandle->pGPIOx->OSPEEDR &= ~(3U << (2U * pin));
	pGPIOHandle->pGPIOx->OSPEEDR |= temp;
	temp = 0;

	//3. Configure the Pull Up - Pull Down Settings
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdCtrl << ( 2U * pin));
	pGPIOHandle->pGPIOx->PUPDR &= ~(3U << (2U * pin));
	pGPIOHandle->pGPIOx->PUPDR |= temp;
	temp = 0;

	//4. Configure the Output Type
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << (pin));
	pGPIOHandle->pGPIOx->OTYPER &= ~(1U << (pin));
	pGPIOHandle->pGPIOx->OTYPER |= temp;
	temp = 0;

	//5. Configure the Alternate functionatily
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_AF){
		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber <= 7){
			temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFuncMode << (4U * pin));
			pGPIOHandle->pGPIOx->AFRL &= ~(15U << (4U * pin));
			pGPIOHandle->pGPIOx->AFRL |= temp;
		}else if((pin >= 8) && (pin <= 15)){
			temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFuncMode << (4U * (pin - 8U)));
			pGPIOHandle->pGPIOx->AFRH &= ~(15U << (4U * (pin - 8U)));
			pGPIOHandle->pGPIOx->AFRH |= temp;
		}
	}
	temp = 0;
}

/****************************************************************************
 * @fn				- GPIO_DeInit
 *
 * @brief			- This function deinitializes the GPIO Port by using RCC peripheral reset register AHB4RSTR
 *
 * @param[in]		- base address of the GPIO port
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- none
 *
 */
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx){
	uintptr_t gpio_addr = (uintptr_t)pGPIOx;
	uint32_t port_index;

	//GPIOA to GPIOP converted numerically 0 to 12
	if((gpio_addr >= GPIOA_BASEADDR) && (gpio_addr <= GPIOH_BASEADDR)){
		port_index = (gpio_addr - GPIOA_BASEADDR) / 0x400UL;
	} else if ((gpio_addr >= GPIOM_BASEADDR) && (gpio_addr <= GPIOP_BASEADDR)){
		port_index = (gpio_addr - GPIOA_BASEADDR) / 0x400UL;
	} else{
		//Error invalid port address
		return;
	}

	//reset and remove reset
	RCC->AHB4RSTR |= (1U << port_index);
	RCC->AHB4RSTR &= ~(1U << port_index);

}

/*
 * Data Read - Write
 */

/****************************************************************************
 * @fn				- GPIO_ReadFromInputPin
 *
 * @brief			- This function reads from the specified pin on the GPIO port
 *
 * @param[in]		- base address of the GPIO port
 * @param[in]		- pin number
 * @param[in]		-
 *
 * @return			- 1 or 0
 *
 * @Note			- none
 *
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber){

}

/****************************************************************************
 * @fn				- GPIO_ReadFromInputPort
 *
 * @brief			- This function reads from the specified GPIO port
 *
 * @param[in]		- base address of the GPIO port
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- 16 bit value (whole port length)
 *
 * @Note			- none
 *
 */
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx){

}

/****************************************************************************
 * @fn				- GPIO_WriteToOutputPin
 *
 * @brief			- This function writes to the specified pin on the GPIO port
 *
 * @param[in]		- base address of the GPIO port
 * @param[in]		- pin number
 * @param[in]		- SET or RESET macros
 *
 * @return			- none
 *
 * @Note			- none
 *
 */
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value){

}

/****************************************************************************
 * @fn				- GPIO_WriteToOutputPort
 *
 * @brief			- This function writes to the specified GPIO port
 *
 * @param[in]		- base address of the GPIO port
 * @param[in]		- 16 bit value to write
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- none
 *
 */
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value){

}

/****************************************************************************
 * @fn				- GPIO_ToggleOutputPin
 *
 * @brief			- This function toggles the specified pin on the GPIO port
 *
 * @param[in]		- base address of the GPIO port
 * @param[in]		- pin number
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- none
 *
 */
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber){

}

/*
 * IRQ Configuration and ISR Handling
 */

/****************************************************************************
 * @fn				- GPIO_IRQConfig
 *
 * @brief			- This function configures the IRQ settings for GPIOs
 *
 * @param[in]		- IRQ Number
 * @param[in]		- priority for the IRQ
 * @param[in]		- ENABLE or DISABLE macros
 *
 * @return			- none
 *
 * @Note			- none
 *
 */
void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t ENorDI){

}

/****************************************************************************
 * @fn				- GPIO_IRQConfig
 *
 * @brief			- This function handles the IRS for the triggered IRQ
 *
 * @param[in]		- pin number
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- none
 *
 */
void GPIO_IRQHandling(uint8_t PinNumber){
	//what happens if irq triggered
}
