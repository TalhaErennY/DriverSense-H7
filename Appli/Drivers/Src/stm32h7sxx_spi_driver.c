/*
 * stm32h7sxx_spi_driver.c
 *
 *  Created on: 7 Eki 2026
 *      Author: talha
 */

#include <stdio.h>
#include "stm32h7sxx_spi_driver.h"
#include "stm32h7sxx_gpio_driver.h"

volatile uint8_t dbg_spi_step = 0U;

/*
 * Private Function Prototypes
 */
static uint8_t SPI_WaitForFlag(SPI_RegDef_t *pSPIx, uint32_t FlagName);

//=======================================================================================

/*
 * Peripheral Clock Setup
 */
/*
 * @fn				- SPI_PeriClockControl
 *
 * @brief			- This function enables or disables peripheral clock for given SPI peripheral
 *
 * @param[in]		- base address of the SPI peripheral
 * @param[in]		- ENABLE or DISABLE macros
 *
 * @return			- none
 */
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t ENorDI){
	    if(ENorDI == ENABLE){
	        if (pSPIx == SPI1){
	            SPI1_PCLK_EN();
	        } else if(pSPIx == SPI2){
	        	SPI2_PCLK_EN();
	        } else if(pSPIx == SPI3){
	        	SPI3_PCLK_EN();
	        } else if(pSPIx == SPI4){
	        	SPI4_PCLK_EN();
	        } else if(pSPIx == SPI5){
	        	SPI5_PCLK_EN();
	        } else if(pSPIx == SPI6){
	        	SPI6_PCLK_EN();
	        }
	    } else if(ENorDI == DISABLE) {
	        if (pSPIx == SPI1){
	        	SPI1_PCLK_DI();
	        } else if(pSPIx == SPI2){
	        	SPI2_PCLK_DI();
	        } else if(pSPIx == SPI3){
	        	SPI3_PCLK_DI();
	        } else if(pSPIx == SPI4){
	        	SPI4_PCLK_DI();
	        } else if(pSPIx == SPI5){
	        	SPI5_PCLK_DI();
	        } else if(pSPIx == SPI6){
	        	SPI6_PCLK_DI();
	        }
	    } else{
	    	//Return error
	    }
}

/*
 * Init - DeInit
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
void SPI_Init(SPI_Handle_t *pSPIHandle){
	//Temporary Register Values
	uint32_t tempCFG1 = 0U;
	uint32_t tempCFG2 = 0U;
	uint32_t tempCR1  = 0U;

	//Enable SPI Peripheral Clock
	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);

	//Master-Slave
	tempCFG2 |= (pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CFG2_MASTER);

	//Communication Mode
	tempCFG2 &= ~(SPI_CFG2_COMM_MSK);
	tempCFG2 |= (pSPIHandle->SPIConfig.SPI_BusConfig << SPI_CFG2_COMM);

	//Master Baudrate Prescaler
	tempCFG1 &= ~(SPI_CFG1_MBR_MSK);
	tempCFG1 |= (pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CFG1_MBR);

	//Data Size
	tempCFG1 &= ~(SPI_CFG1_DSIZE_MSK);
	tempCFG1 |= (pSPIHandle->SPIConfig.SPI_DataSize << SPI_CFG1_DSIZE);

	//Clock Polarity
	tempCFG2 |= (pSPIHandle->SPIConfig.SPI_CPOL << SPI_CFG2_CPOL);

	//Clock Phase
	tempCFG2 |= (pSPIHandle->SPIConfig.SPI_CPHA << SPI_CFG2_CPHA);

	//Software Slave Management
	tempCFG2 |= (pSPIHandle->SPIConfig.SPI_SSM << SPI_CFG2_SSM);

	/*
	 * In software slave management mode,
	 * internal NSS must stay inactive when SPI is master.
	 */
	if((pSPIHandle->SPIConfig.SPI_DeviceMode == SPI_DEVICE_MODE_MASTER) &&
	   (pSPIHandle->SPIConfig.SPI_SSM == SPI_SSM_EN)){

		tempCR1 |= (1UL << SPI_CR1_SSI);
	}

	if(pSPIHandle->SPIConfig.SPI_DeviceMode == SPI_DEVICE_MODE_MASTER){

		tempCFG2 |= (1UL << SPI_CFG2_AFCNTR);
	}

	pSPIHandle->pSPIx->CFG1 = tempCFG1;

	/*
	 * Load CR1 before CFG2 so that SSI is already inactive
	 * before master + software NSS management is selected.
	 */
	pSPIHandle->pSPIx->CR1 = tempCR1;

	pSPIHandle->pSPIx->CFG2 = tempCFG2;

}

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
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t ENorDI){

	if(ENorDI == ENABLE){
		pSPIx->CR1 |= (1UL << SPI_CR1_SPE);
	}
	else{
		pSPIx->CR1 &= ~(1UL << SPI_CR1_SPE);
	}
}

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
void SPI_StartCommunication(SPI_RegDef_t *pSPIx){

	pSPIx->CR1 |= (1UL << SPI_CR1_CSTART);
}

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
void SPI_DeInit(SPI_RegDef_t *pSPIx);



/*
 * Data Send and Receive
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
 * @Note			- If you use to transfer then write NULL to rxbuffer field, this also reads the rxdr to empyt rxfifo and vice versa for tx
 *
 */
uint8_t SPI_Transfer(SPI_RegDef_t *pSPIx,
					 uint8_t *pTxBuffer,
					 uint8_t *pRxBuffer,
					 uint32_t Len){

	uint8_t txData;
	uint8_t rxData;

	if((pSPIx == NULL) || (Len == 0U))
		return SPI_ERROR_INVALID_ARG;

	if(Len > 0xFFFFU)
		return SPI_ERROR_INVALID_ARG;


	/*
	 * SPI must be disabled before TSIZE is changed.
	 */
	SPI_PeripheralControl(pSPIx, DISABLE);


	/*
	 * Set Transfer Size.
	 */
	pSPIx->CR2 &= ~(SPI_CR2_TSIZE_MSK);
	pSPIx->CR2 |= (Len << SPI_CR2_TSIZE);


	/*
	 * Enable SPI.
	 */
	SPI_PeripheralControl(pSPIx, ENABLE);


	/*
	 * Start Master Transfer.
	 */
	SPI_StartCommunication(pSPIx);


	while(Len > 0U){

		/*
		 * Wait until TxFIFO has space.
		 */
		if(SPI_WaitForFlag(pSPIx, SPI_FLAG_TXP) != SPI_OK){

			SPI_PeripheralControl(pSPIx, DISABLE);

			return SPI_ERROR_TIMEOUT;
		}


		if(pTxBuffer != NULL){

			txData = *pTxBuffer++;
		}
		else{

			txData = SPI_DUMMY_BYTE;
		}


		/*
		 * DSIZE = 8 bit.
		 */
		*((__vo uint8_t *)&pSPIx->TXDR) = txData;


		/*
		 * Wait for received byte.
		 */
		if(SPI_WaitForFlag(pSPIx, SPI_FLAG_RXP) != SPI_OK){

			SPI_PeripheralControl(pSPIx, DISABLE);

			return SPI_ERROR_TIMEOUT;
		}


		rxData = *((__vo uint8_t *)&pSPIx->RXDR);


		if(pRxBuffer != NULL){

			*pRxBuffer++ = rxData;
		}


		Len--;
	}


	/*
	 * Wait for complete transfer.
	 */
	if(SPI_WaitForFlag(pSPIx, SPI_FLAG_EOT) != SPI_OK){

		SPI_PeripheralControl(pSPIx, DISABLE);

		return SPI_ERROR_TIMEOUT;
	}


	/*
	 * Clear transfer flags.
	 */
	pSPIx->IFCR =
			(1UL << SPI_IFCR_EOTC) |
			(1UL << SPI_IFCR_TXTFC);


	/*
	 * End transfer.
	 *
	 * AFCNTR keeps SPI output pins under peripheral
	 * control while SPE is disabled.
	 */
	SPI_PeripheralControl(pSPIx, DISABLE);


	return SPI_OK;
}

uint8_t SPI_TransferByte(SPI_RegDef_t *pSPIx, uint8_t TxData, uint8_t *pRxData){
	return SPI_Transfer(pSPIx, &TxData, pRxData, 1U);
}

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
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName){
	if(pSPIx->SR & FlagName){
		return FLAG_SET;
	}

	return FLAG_RESET;
}

/*
 * IRQ Configuration and ISR Handling
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
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t ENorDI);
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
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);
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
void SPI_IRQHandling(SPI_Handle_t *pSPIHandle);

/*
 * Easy Access Functions
 */
/*
 * SPI1 GPIO Init
 */
void SPI2S1_GPIOInits(void){
	GPIO_Handle_t SPI1Pins_A;
	GPIO_Handle_t SPI1Pins_B;

	SPI1Pins_A.pGPIOx = GPIOA;
	SPI1Pins_B.pGPIOx = GPIOB;

	/*
	 * GPIOA Configuration
	 */
	SPI1Pins_A.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_AF;
	SPI1Pins_A.GPIO_PinConfig.GPIO_PinAltFuncMode = 5U;
	SPI1Pins_A.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPI1Pins_A.GPIO_PinConfig.GPIO_PinPuPdCtrl = GPIO_PUPD_PU;
	SPI1Pins_A.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;

	/*
	 * GPIOB Configuration
	 */
	SPI1Pins_B.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_AF;
	SPI1Pins_B.GPIO_PinConfig.GPIO_PinAltFuncMode = 5U;
	SPI1Pins_B.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPI1Pins_B.GPIO_PinConfig.GPIO_PinPuPdCtrl = GPIO_PUPD_PU;
	SPI1Pins_B.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;

	/*
	 * Enable GPIO Clocks
	 */
	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_PeriClockControl(GPIOB, ENABLE);

	/*
	 * PA5 -> SPI1_SCK
	 */
	SPI1Pins_A.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GPIO_Init(&SPI1Pins_A);

	/*
	 * PA6 -> SPI1_MISO
	 */
	SPI1Pins_A.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&SPI1Pins_A);

	/*
	 * PB5 -> SPI1_MOSI
	 */
	SPI1Pins_B.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GPIO_Init(&SPI1Pins_B);
}

/*
 * Private Helpers
 */
static uint8_t SPI_WaitForFlag(SPI_RegDef_t *pSPIx, uint32_t FlagName){

	uint32_t timeout = SPI_TIMEOUT;

	while(SPI_GetFlagStatus(pSPIx, FlagName) == FLAG_RESET){

		if(timeout == 0U){
			return SPI_ERROR_TIMEOUT;
		}

		timeout--;
	}

	return SPI_OK;
}
