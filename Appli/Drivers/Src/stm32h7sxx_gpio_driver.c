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
 * @Note			- GPIO ports are spaced by 0x400 bytes.
 * 					- RCC AHB4ENR/AHB4RSTR bit positions match the port index:
 *					- GPIOA=0, GPIOB=1, ..., GPIOH=7, GPIOM=12, ..., GPIOP=15.
 *
 */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t ENorDI){
	uintptr_t gpio_addr = (uintptr_t)pGPIOx;
	uint32_t port_index;

	if((gpio_addr >= GPIOA_BASEADDR) && (gpio_addr <= GPIOH_BASEADDR)){
		if(((gpio_addr - GPIOA_BASEADDR) % 0x400UL) != 0U){
		    return;
		}

		port_index = (gpio_addr - GPIOA_BASEADDR) / 0x400UL;
	} else if ((gpio_addr >= GPIOM_BASEADDR) && (gpio_addr <= GPIOP_BASEADDR)){

		if(((gpio_addr - GPIOM_BASEADDR) % 0x400UL) != 0U){
	        return;
	    }

	    port_index = 12U + ((gpio_addr - GPIOM_BASEADDR) / 0x400UL);
	} else{
		//Error invalid port address
		return;
	}

	if(ENorDI == ENABLE){
		RCC->AHB4ENR |= (1UL << port_index);
	} else if(ENorDI == DISABLE){
		RCC->AHB4ENR &= ~(1UL << port_index);
	} else {
		//Only ENABLE or DISABLE can be written
		return;
	}
}

/*
 * GPIO Init - DeInit
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
 * @Note			- For GPIO_PinMode, GPIO_PinSpeed, GPIO_PinPuPdCtrl, GPIO_PinOPType and
 * 					  GPIO_PinAltFuncMode selections, see @GPIO_PIN_MODES, @GPIO_OUTPUT_SPEEDS,
 * 					  @GPIO_PUPD_CONFIGS, @GPIO_OUTPUT_TYPES and device alternate function table.
 * 					- GPIO ports are spaced by 0x400 bytes.
 * 					- SBS_EXTICRx bit values match the port indexes:
 *					- PA=0, PB=1, ..., PH=7, PM=12, ..., PP=15.
 *
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle){
	if(pGPIOHandle == 0) return; //Unknown handle config

	//Temp register
	uint32_t temp = 0;
	uint32_t pin = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber;

	if(pin > 15U) return;		 //Unknown pin number

	//1. Configure the mode of GPIO Pin
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG){
		//non-interrupt modes
		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << ( 2U * pin));
		pGPIOHandle->pGPIOx->MODER &= ~(3U << (2U * pin));
		pGPIOHandle->pGPIOx->MODER |= temp;
	} else{
		//interrupt modes

		//Automatically Changing the mode to input mode for interrupt handling
		pGPIOHandle->pGPIOx->MODER &= ~(3UL << (2U * pin));   			// input mode = 00

		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT){
			//Falling edge trigger configuration
			//1. Configure the FTSR
			EXTI->FTSR1 |= (1U << pin);
			//Clear the corresponding RTSR bit
			EXTI->RTSR1 &= ~(1U << pin);
		} else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT){
			//Rising edge trigger configuration
			//1. Configure the RTSR
			EXTI->RTSR1 |= (1U << pin);
			//Clear the corresponding FTSR bit
			EXTI->FTSR1 &= ~(1U << pin);
		} else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT){
			//Rising and Falling edge trigger configuration
			//1. Configure both FTSR and RTSR
			EXTI->RTSR1 |= (1U << pin);
			EXTI->FTSR1 |= (1U << pin);
		} else{
			//Invalid IRQ mode config or invalid edge trigger
			return;
		}

		//2. Configure the GPIO port selection in SBS_EXTICR
		//Automatically activates the APB4 clock for SBS
		SBS_PCLK_EN();

		uintptr_t gpio_addr = (uintptr_t)pGPIOHandle->pGPIOx;
		uint32_t port_index;

		if((gpio_addr >= GPIOA_BASEADDR) && (gpio_addr <= GPIOH_BASEADDR)){
			if(((gpio_addr - GPIOA_BASEADDR) % 0x400UL) != 0U){
				return;
			}

			port_index = (gpio_addr - GPIOA_BASEADDR) / 0x400UL;
		} else if ((gpio_addr >= GPIOM_BASEADDR) && (gpio_addr <= GPIOP_BASEADDR)){
		    if(((gpio_addr - GPIOM_BASEADDR) % 0x400UL) != 0U){
		        return;
		    }

		    port_index = 12U + ((gpio_addr - GPIOM_BASEADDR) / 0x400UL);
		} else{
			//Error invalid port address
			return;
		}

		uint32_t exticr_index = (pin / 4U);
		uint32_t exticr_shift = ((pin % 4U) * 4U);

		SBS->EXTICR[exticr_index] &= ~(0xFUL << exticr_shift);
		SBS->EXTICR[exticr_index] |= (port_index << exticr_shift);

		//3. Enable the EXTI interrupt delivery using IMR
		EXTI->IMR1 |= (1U << pin);

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

	//5. Configure the Alternate functionality
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
 * @Note			- GPIO ports are spaced by 0x400 bytes.
 * 					- RCC AHB4ENR/AHB4RSTR bit positions match the port index:
 *					- GPIOA=0, GPIOB=1, ..., GPIOH=7, GPIOM=12, ..., GPIOP=15.
 *
 */
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx){
	uintptr_t gpio_addr = (uintptr_t)pGPIOx;
	uint32_t port_index;

	//GPIOA-GPIOH and GPIOM-GPIOP converted to AHB4RSTR/AHB4ENR bit positions
	if((gpio_addr >= GPIOA_BASEADDR) && (gpio_addr <= GPIOH_BASEADDR)){
		if(((gpio_addr - GPIOA_BASEADDR) % 0x400UL) != 0U){
		    return;
		}

		port_index = (gpio_addr - GPIOA_BASEADDR) / 0x400UL;
	} else if ((gpio_addr >= GPIOM_BASEADDR) && (gpio_addr <= GPIOP_BASEADDR)){
	    if(((gpio_addr - GPIOM_BASEADDR) % 0x400UL) != 0U){
	        return;
	    }

	    port_index = 12U + ((gpio_addr - GPIOM_BASEADDR) / 0x400UL);
	} else{
		//Error invalid port address
		return;
	}

	//Reset and remove reset
	RCC->AHB4RSTR |= (1UL << port_index);
	RCC->AHB4RSTR &= ~(1UL << port_index);
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
	if(pGPIOx == 0 || PinNumber > 15U) return 0;   //Invalid pin number/GPIO address

	return (uint8_t)((pGPIOx->IDR >> PinNumber) & 1U);
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
	if(pGPIOx == 0) return 0; //Invalid GPIO address

	return (uint16_t)(pGPIOx->IDR & 0xFFFFU);
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
 * @Note			- For Value selection, see GPIO_PIN_SET / GPIO_PIN_RESET macros in stm32h7sxx.h.
 * 					- This function uses BSRR for atomic bit write
 *
 */
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value){
	if(pGPIOx == 0 || PinNumber > 15U) return;   //Invalid pin number/GPIO address

	if(Value == GPIO_PIN_SET){
		pGPIOx->BSRR = (1UL << PinNumber);
	} else if(Value == GPIO_PIN_RESET){
		pGPIOx->BSRR = (1UL << (PinNumber + 16U));
	} else{
		//error
	}
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
	if(pGPIOx == 0) return;  //Invalid GPIO address

	pGPIOx->ODR = (pGPIOx->ODR & ~0xFFFFU) | ((uint32_t)Value & 0xFFFFU);
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
	if(pGPIOx == 0 || PinNumber > 15U) return;   //Invalid pin number/GPIO address

	pGPIOx->ODR ^= (1UL << PinNumber);
}

/*
 * IRQ Configuration and ISR Handling
 */

/****************************************************************************
 * @fn				- GPIO_IRQInterruptConfig
 *
 * @brief			- This function configures the IRQ settings for GPIOs
 *
 * @param[in]		- IRQ Number
 * @param[in]		- ENABLE or DISABLE macros
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- For ENorDI selection, see ENABLE / DISABLE macros in stm32h7sxx.h.
 *
 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t ENorDI){
	if(IRQNumber > 155U) return;		//Invalid IRQ Number

	if(ENorDI == ENABLE){
		//Program ISERx Register
		NVIC_ISER[IRQNumber / 32U] = (1U << (IRQNumber % 32U));
	} else if(ENorDI == DISABLE){
		//Program ICERx Register
		NVIC_ICER[IRQNumber / 32U] = (1U << (IRQNumber % 32U));
	} else{
		//Invalid value for ENABLE or DISABLE
		return;
	}
}

/****************************************************************************
 * @fn				- GPIO_IRQPriorityConfig
 *
 * @brief			- This function configures the IRQ priority setting for IRQs
 *
 * @param[in]		- IRQ Number
 * @param[in]		- IRQ priority
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- IRQPriority must match implemented NVIC priority bits.
 *
 */
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority){
	if(IRQNumber > 155U) return;		//Invalid IRQ Number
	if(IRQPriority > 15U) return;		//Invalid IRQ Priority

	NVIC_IPR[IRQNumber] = (IRQPriority << NVIC_PRIORITY_SHIFT);
}

/****************************************************************************
 * @fn				- GPIO_IRQHandling
 *
 * @brief			- This function handles the ISR for the triggered IRQ
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
	//If the event occurred clear the pending register for the corresponding bit
	if(PinNumber > 15U) return;

	if(EXTI->PR1 & (1UL << PinNumber)){
		EXTI->PR1 = (1UL << PinNumber);
	}
}

