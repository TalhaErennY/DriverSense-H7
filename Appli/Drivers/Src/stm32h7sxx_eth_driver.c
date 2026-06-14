/*
 * stm32h7sxx_eth_driver.c
 *
 *  Created on: 7 Haz 2026
 *      Author: talha
 */

#include "stm32h7sxx_eth_driver.h"
#include "stm32h7sxx_gpio_driver.h"
#include "stm32h7sxx.h"

/******************************************************************************************************
 * 								Private Driver Variables
 ******************************************************************************************************/
/*
 * RX/TX descriptors and RX buffers
 *
 * This section is placed into ETH_DMA_RAM.
 * MPU marks this region as non-cacheable.
 */
__attribute__((used, section(".eth_dma"), aligned(32)))
ETH_DMADesc_t ETH_RX_Desc[ETH_RX_DESC_CNT];

__attribute__((used, section(".eth_dma"), aligned(32)))
ETH_DMADesc_t ETH_TX_Desc[ETH_TX_DESC_CNT];

__attribute__((used, section(".eth_dma"), aligned(32)))
uint8_t ETH_RX_Buffers[ETH_RX_DESC_CNT][ETH_RX_BUF_SIZE];


/*
 * TX buffers
 *
 * This section is placed into ETH_TX_RAM.
 * It is outside the MPU non-cacheable ETH DMA region.
 * Therefore it stays cacheable.
 */
__attribute__((used, section(".eth_tx_buffer"), aligned(32)))
uint8_t ETH_TX_Buffers[ETH_TX_DESC_CNT][ETH_TX_BUF_SIZE];

/*
 * Current RX/TX descriptor index.
 *
 * This variable is private to Ethernet driver.
 */
static uint32_t ETH_RX_CurrentDesc = 0U;
static uint32_t ETH_TX_CurrentDesc = 0U;


/******************************************************************************************************
 * 								Private Helper Functions
 ******************************************************************************************************/

/*
 * DSB = Data Synchronization Barrier
 *
 * Finish memory/system register operations before continuing
 */
static void CPU_DSB(void){
	__asm volatile ("dsb 0xF" ::: "memory");
}

/*
 * ISB = Instruction Synchronization Barrier
 *
 * Reload CPU execution pipeline after system control changes
 */
static void CPU_ISB(void){
	__asm volatile ("isb 0xF" ::: "memory");
}

/****************************************************************************
 * @fn				- ETH_MDIO_WaitReady
 *
 * @brief			- This internal helper waits until MDIO interface becomes ready
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- 1 if MDIO interface is ready, 0 if timeout occurs
 *
 * @Note			- ETH_MACMDIOAR.MB bit is set while an MDIO transaction is ongoing.
 * 					- This function is private to this driver file.
 *					-
 *
 */
static uint8_t ETH_MDIO_WaitReady(void){
	uint32_t timeout = ETH_TIMEOUT_VALUE;

	while((ETH->MACMDIOAR & (1UL << ETH_MACMDIOAR_MB)) != 0U){
		if(--timeout == 0U){
			return 0U;
		}
	}

	return 1U;
}


static void ETH_CleanDCacheByAddr(uint32_t addr, uint32_t len){
	uint32_t clean_addr;
	uint32_t clean_len;
	uint32_t end_addr;

	if(len == 0U) return;

	clean_addr = ETH_ALIGN_DOWN(addr);
	clean_len  = ETH_ALIGN_UP(len + (addr - clean_addr));
	end_addr   = clean_addr + clean_len;

	CPU_DSB();

	while(clean_addr < end_addr){
		SCB_DCCMVAC = clean_addr;
		clean_addr += ETH_CACHE_LINE_SIZE;
	}

	CPU_DSB();
	CPU_ISB();
}

/******************************************************************************************************
 * 								Peripheral Clock / Interface / Reset Setup
 ******************************************************************************************************/

/************************************************************************************
 * @fn				- ETH_PeriClockControl
 *
 * @brief			- This function enables or disables peripheral clock for ETH MAC,
 * 					  ETH TX and ETH RX blocks
 *
 * @param[in]		- ENABLE or DISABLE macros
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- For ENorDI selection, see ENABLE / DISABLE macros in stm32h7sxx.h.
 *					- Ethernet peripheral has separate clock enable bits for MAC,
 * 					  TX and RX paths.
 * 					- All three clocks must be enabled before using ETH registers.
 *					-
 *
 */
void ETH_PeriClockControl(uint8_t ENorDI){
	if(ENorDI == ENABLE){
		ETH1_PCLK_EN();
	}else{
		ETH1_PCLK_DI();
	}
}

/*********************************************************************************
 * @fn				- ETH_GPIO_RMII_Init
 *
 * @brief			- This function configures GPIO pins used by Ethernet RMII interface
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- RMII pins on NUCLEO-H7S3L8:
 * 					- PB6  -> ETH_REF_CLK
 *					- PA2  -> ETH_MDIO
 *					- PG6  -> ETH_MDC
 *					- PA7  -> ETH_CRS_DV
 *					- PG4  -> ETH_RXD0
 *					- PG5  -> ETH_RXD1
 *					- PG11 -> ETH_TX_EN
 *					- PG13 -> ETH_TXD0
 *					- PG12 -> ETH_TXD1
 *
 */
void ETH_GPIO_RMII_Init(void){
	GPIO_Handle_t GPIOETH;

	//Enable GPIO clocks
	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_PeriClockControl(GPIOB, ENABLE);
	GPIO_PeriClockControl(GPIOG, ENABLE);

	//GPIO Configurations
	GPIOETH.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_AF;
	GPIOETH.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GPIOETH.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_VHIGH;
	GPIOETH.GPIO_PinConfig.GPIO_PinPuPdCtrl = GPIO_PUPD_NONE;
	GPIOETH.GPIO_PinConfig.GPIO_PinAltFuncMode = 11U;				//For ethernet related things

	/*
	 * GPIOA Pins:
	 *
	 * PA2 -> ETH_MDIO
	 * PA7 -> ETH_CRS_DV
	 */
	GPIOETH.pGPIOx = GPIOA;

	GPIOETH.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2;
	GPIO_Init(&GPIOETH);

	GPIOETH.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Init(&GPIOETH);

	/*
	 * GPIOB Pin:
	 *
	 * PB6 -> ETH_REF_CLK
	 */
	GPIOETH.pGPIOx = GPIOB;

	GPIOETH.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&GPIOETH);

	/*
	 * GPIOG Pins:
	 *
	 * PG4  -> ETH_RXD0
	 * PG5  -> ETH_RXD1
	 * PG6  -> ETH_MDC
	 * PG11 -> ETH_TX_EN
	 * PG12 -> ETH_TXD1
	 * PG13 -> ETH_TXD0
	 */
	GPIOETH.pGPIOx = GPIOG;

	GPIOETH.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_4;
	GPIO_Init(&GPIOETH);

	GPIOETH.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GPIO_Init(&GPIOETH);

	GPIOETH.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&GPIOETH);

	GPIOETH.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_11;
	GPIO_Init(&GPIOETH);

	GPIOETH.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&GPIOETH);

	GPIOETH.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&GPIOETH);
}

/****************************************************************************
 * @fn				- ETH_SelectRMII
 *
 * @brief			- This function selects RMII as the Ethernet PHY interface
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- On STM32H7Rx/7Sx, Ethernet PHY interface is selected using
 * 					  SBS_PMCR.ETH_PHYSEL[2:0].
 * 					- ETH_PHYSEL = 100 selects RMII.
 *					- This function must be called before Ethernet software reset.
 *					- (GMII/MII support will come soon...)
 *
 */
void ETH_SelectRMII(void){
	SBS_PCLK_EN();
	/*
	 * Clear ETH_PHYSEL[2:0] field first.
	 */
	SBS->PMCR &= ~(0x7UL << SBS_PMCR_ETH_PHYSEL);

	/*
	 * Select RMII mode: ETH_PHYSEL[2:0] = 100.
	 */
	SBS->PMCR |= ((uint32_t)SBS_PMCR_ETH_PHYSEL_RMII << SBS_PMCR_ETH_PHYSEL);
}

/****************************************************************************
 * @fn				- ETH_SoftwareReset
 *
 * @brief			- This function performs Ethernet DMA software reset
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- 1 for Successful, 0 for not
 *
 * @Note			- ETH_DMAMR.SWR bit is set by software and cleared by hardware
 * 					  when reset is completed.
 * 					- If this function gets stuck, ETH clock or RMII reference clock
 * 					  may be missing.
 *					-
 *
 */
uint8_t ETH_SoftwareReset(void){
	uint32_t timeout = ETH_TIMEOUT_VALUE;

	ETH->DMAMR |= (1UL << ETH_DMAMR_SWR);

	while((ETH->DMAMR & (1UL << ETH_DMAMR_SWR)) != 0U){
		if(--timeout == 0U){
			return 0U;
		}
	}

	return 1U;
}

/******************************************************************************************************
 * 								MPU / Cache Configuration
 ******************************************************************************************************/

/****************************************************************************
 * @fn				- ETH_MPU_ConfigNonCacheable
 *
 * @brief			- This function configures a 16 KB non-cacheable MPU region
 * 					  for Ethernet DMA descriptors and buffers
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- Ethernet DMA writes RX packets directly into RAM.
 * 					- If RX buffers are cacheable, CPU may read old cached data.
 *					- This function maps 0x24040000 - 0x24043FFF as normal,
 *					  shareable, non-cacheable, non-executable memory.
 *
 */
void ETH_MPU_ConfigNonCacheable(void){
	/*
	 * Disable MPU before changing region configuration
	 */
	MPU->CTRL &= ~(1UL << MPU_CTRL_ENABLE);

	CPU_DSB();
	CPU_ISB();

	/*
	 * Select MPU region number 0
	 */
	MPU->RNR = 0U;

	/*
	 * Region base address
	 *
	 * ETH_DMA_RAM starts at 0x24040000
	 * Size: 16 KB (0x4000)
	 * Alignment = 32 bytes
	 */
	MPU->RBAR = 0x24040000UL;

	/*
	 * MPU_RASR configuration:
	 *
	 * XN	  = 1		Execute Never: Never execute instructions from this buffer region.
	 * AP	  = 011		Full access
	 * TEX	  = 001		Normal memory type
	 * S	  = 1		Shareable: For DMA-shared memory, this memory is shared between CPU and DMA
	 * C	  = 0		Non-cacheable
	 * B	  = 0		Non-bufferable
	 * SIZE	  = 13		2^(size+1) -> 16KB with 13
	 * ENABLE = 1		Enable this MPU region
	 */
	MPU->RASR =
			((1UL  << MPU_RASR_XN)   |
			 (3UL  << MPU_RASR_AP)   |
			 (1UL  << MPU_RASR_TEX)  |
			 (1UL  << MPU_RASR_S)	 |
			 (13UL << MPU_RASR_SIZE) |
			 (1UL  << MPU_RASR_ENABLE));

	/*
	 * Enable MPU
	 */
	MPU->CTRL = ((1UL << MPU_CTRL_ENABLE) | (1UL << MPU_CTRL_PRIVDEFENA));

	CPU_DSB();
	CPU_ISB();
}

/******************************************************************************************************
 * 								MAC Configuration
 ******************************************************************************************************/

/****************************************************************************
 * @fn				- ETH_SetMACAddress
 *
 * @brief			- This function programs MAC address 0 into Ethernet MAC registers
 *
 * @param[in]		- pointer to 6-byte MAC address array
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- Example MAC address: 02:00:00:00:00:01
 * 					- 0x02 as first byte means locally administered unicast MAC.
 *					-
 *
 */
void ETH_SetMACAddress(const uint8_t *pMACAddr){
	if(pMACAddr == 0) return;

	/*
	 * MACA0LR contains MAC address byte 0-3
	 */
	ETH->MACA0LR = (((uint32_t)pMACAddr[0])		  |
				    ((uint32_t)pMACAddr[1] << 8)  |
				    ((uint32_t)pMACAddr[2] << 16) |
				    ((uint32_t)pMACAddr[3] << 24));

	/*
	 * MACA0HR contains MAC address bytes 4-5
	 *
	 * Bit 31 is address enable bit
	 */
	ETH->MACA0HR = (((uint32_t)pMACAddr[4]) 	  |
					((uint32_t)pMACAddr[5] << 8)  |
					(1UL << 31));
}

/****************************************************************************
 * @fn				- ETH_MAC_FilterConfig
 *
 * @brief			- This function configures MAC packet filtering
 *
 * @param[in]		- Value: pre-shifted bit value to be written
 * @param[in]		- ModeMask: selection from @ETH_MACPFR_FILTERS mask macros
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			-
 * 					-
 *					-
 *
 */
void ETH_MAC_FilterConfig(uint32_t Value, uint32_t FilterMask){
	ETH->MACPFR &= ~(FilterMask);
	ETH->MACPFR |= (Value & FilterMask);
}

/****************************************************************************
 * @fn				- ETH_MAC_OPModeConfig
 *
 * @brief			- The MAC Configuration register establishes the operating mode of the MAC.
 *
 * @param[in]		- Value: pre-shifted bit value to be written
 * @param[in]		- ModeMask: selection from @ETH_MACCR_MODES mask macros
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- DMA should be started before enabling MAC.
 * 					- This sets ETH_MACCR bits.
 *					-
 *
 */
void ETH_MAC_OPModeConfig(uint32_t Value, uint32_t ModeMask){
	ETH->MACCR &= ~(ModeMask);
	ETH->MACCR |= (Value & ModeMask);
}

/******************************************************************************************************
 * 								MTL Configuration
 ******************************************************************************************************/
/****************************************************************************
 * @fn				- ETH_MTL_RXQueueConfig
 *
 * @brief			- This function configures Ethernet MTL RX queue
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- Store-and-forward mode is used for simpler RX operation.
 * 					- In this mode, MTL stores a complete frame before forwarding
 * 					  it to DMA.
 *					-
 *
 */
void ETH_MTL_RXQueueConfig(uint32_t Value, uint32_t ModeMask){
	ETH->MTLRXQOMR &= ~(ModeMask);
	ETH->MTLRXQOMR |= (Value & ModeMask);
}

/****************************************************************************
 * @fn				- ETH_MTL_TXQueueConfig
 *
 * @brief			- This function configures Ethernet MTL TX queue
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- Store-and-forward mode is used for simpler TX operation.
 *					-
 *
 */
void ETH_MTL_TXQueueConfig(uint32_t Value, uint32_t ModeMask){
	ETH->MTLTXQOMR &= ~(ModeMask);
	ETH->MTLTXQOMR |= (Value & ModeMask);
}

/******************************************************************************************************
 * 								TX DMA Descriptor / TX Path Configuration
 ******************************************************************************************************/
/****************************************************************************
 * @fn				- ETH_TXDesc_Init
 *
 * @brief			- This function initializes Ethernet TX DMA descriptors and buffers
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- Each TX descriptor points to one TX buffer.
 * 					- DESC0 contains buffer address.
 *					- DESC3 is configured with OWN, IOC and BUF1V bits.
 *					- OWN = 1 gives the descriptor to DMA.
 *
 */
void ETH_TXDesc_Init(void){
	for(uint32_t i = 0U; i < ETH_TX_DESC_CNT; i++){
		/*
		 * Clear TX Buffer
		 */
		for(uint32_t j = 0U; j < ETH_TX_BUF_SIZE; j++){
			ETH_TX_Buffers[i][j] = 0U;
		}

		/*
		 * TX Descriptors start as CPU Owned
		 */
		ETH_TX_Desc[i].DESC0 = 0U;
		ETH_TX_Desc[i].DESC1 = 0U;
		ETH_TX_Desc[i].DESC2 = 0U;
		ETH_TX_Desc[i].DESC3 = 0U;
	}

	ETH_TX_CurrentDesc = 0U;

	CPU_DSB();
}

/****************************************************************************
 * @fn				- ETH_DMA_TXConfig
 *
 * @brief			- This function configures Ethernet TX DMA descriptor list
 * 					  address, ring length, tail pointer and TX buffer size
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- TX descriptors must already be initialized before calling
 * 					  this function.
 * 					- ETH_TX_Desc is placed in cacheable RAM at 0x24044000.
 *					-
 *
 */
void ETH_DMA_TXConfig(void){
	/*
	 * Channel TX Descriptor ring length register
	 */
	ETH->DMACTXRLR = (ETH_TX_DESC_CNT - 1U);

	/*
	 * Channel TX Descriptor list address register
	 */
	ETH->DMACTXDLAR = (uint32_t)&ETH_TX_Desc[0];

	/*
	 * Channel TX Descriptor tail pointer register
	 */
	ETH->DMACTXDTPR = (uint32_t)&ETH_TX_Desc[0];
}

/****************************************************************************
 * @fn				- ETH_DMA_TXStart
 *
 * @brief			- This function starts Ethernet TX DMA
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- TX descriptors must be owned by DMA before starting TX DMA.
 * 					- This sets ETH_DMACTXCR.SR bit.
 *					-
 *
 */
void ETH_DMA_TXStart(void){
	ETH->DMACTXCR |= (1UL << ETH_DMACTXCR_ST);
}

/****************************************************************************
 * @fn				- ETH_SendRawFrame
 *
 * @brief			- T
 *
 *
 * @param[in]		-
 * @param[in]		-
 * @param[in]		-
 *
 * @return			-
 *
 * @Note			-
 * 					-
 *
 *					-
 *
 */
uint8_t ETH_SendRawFrame(const uint8_t *pFrame, uint32_t FrameLen){
	ETH_DMADesc_t *pDesc;
	uint32_t next_desc;
	uint32_t tx_len;

	if((pFrame == 0) || (FrameLen == 0) || (FrameLen > ETH_TX_BUF_SIZE)) return 0U;

	pDesc = &ETH_TX_Desc[ETH_TX_CurrentDesc];

	/*
	 * If OWN is 1, DMA not finised
	 */
	if(pDesc->DESC3 & ETH_TDES3_OWN) return 0U;

	/*
	 * Ethernet minimum len (60) check and add padding if necessary
	 */
	tx_len = FrameLen;

	if(tx_len < 60U){
		tx_len = 60U;
	}

	/*
	 * Clear TX Buffer for padding area
	 */
	for(uint32_t i = 0U; i < tx_len; i++){
		ETH_TX_Buffers[ETH_TX_CurrentDesc][i] = 0U;
	}

	/*
	 * Copy frame to TX Buffer
	 */
	for(uint32_t i = 0U; i < FrameLen; i++){
		ETH_TX_Buffers[ETH_TX_CurrentDesc][i] = pFrame[i];
	}

	/*
	 * Clean D-cache before giving descriptor to DMA
	 */
	ETH_CleanDCacheByAddr((uint32_t)&ETH_TX_Buffers[ETH_TX_CurrentDesc][0], tx_len);

	/*
	 * Fill TX Descriptors
	 */
	pDesc->DESC0 = (uint32_t)&ETH_TX_Buffers[ETH_TX_CurrentDesc][0];
	pDesc->DESC1 = 0U;
	pDesc->DESC2 = (tx_len & ETH_TDES2_B1L_MASK);

	/*
	 * Buffer and descriptor clean check
	 */
	CPU_DSB();

	/*
	 * OWN Must be written last
	 *
	 * FD = First Descriptor
	 * LD = last descriptor
	 * FL = frame length
	 */
	pDesc->DESC3 = (ETH_TDES3_OWN | ETH_TDES3_FD | ETH_TDES3_LD | (tx_len & ETH_TDES3_FL_MASK));

	CPU_DSB();

	/*
	 * Next descriptor index
	 */
	next_desc = ETH_TX_CurrentDesc + 1U;

	if(next_desc >= ETH_TX_DESC_CNT) next_desc = 0U;

	ETH_TX_CurrentDesc = next_desc;

	/*
	 * New descriptor available
	 */
	ETH->DMACTXDTPR = (uint32_t)&ETH_TX_Desc[next_desc];

	return 1U;
}

/******************************************************************************************************
 * 								RX DMA Descriptor / RX Path Configuration
 ******************************************************************************************************/

/****************************************************************************
 * @fn				- ETH_RXDesc_Init
 *
 * @brief			- This function initializes Ethernet RX DMA descriptors and buffers
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- Each RX descriptor points to one RX buffer.
 * 					- DESC0 contains buffer address.
 *					- DESC3 is configured with OWN, IOC and BUF1V bits.
 *					- OWN = 1 gives the descriptor to DMA.
 *
 */
void ETH_RXDesc_Init(void){
	for(uint32_t i = 0U; i < ETH_RX_DESC_CNT; i++){
		/*
		 * Clear RX Buffer
		 */
		for(uint32_t j = 0U; j < ETH_RX_BUF_SIZE; j++){
				ETH_RX_Buffers[i][j] = 0U;
		}

		/*
		 * RDES0 = Buffer 1 address pointer
		 */
		ETH_RX_Desc[i].DESC0 = (uint32_t)&ETH_RX_Buffers[i][0];

		/*
		 * Buffer 2 is 0
		 */
		ETH_RX_Desc[i].DESC1 = 0U;
		ETH_RX_Desc[i].DESC2 = 0U;

		/*
		 * Descriptor to DMA
		 *
		 * OWN = Dma Owns descriptor
		 * IOC = Interrupt on completion later
		 * BUF1V = Buffer 1 address valid
		 */
		ETH_RX_Desc[i].DESC3 = ETH_RDES3_OWN | ETH_RDES3_IOC | ETH_RDES3_BUF1V;

	}
	//Check if the descriptor writes are complete before DMA uses them
	CPU_DSB();
}

/****************************************************************************
 * @fn				- ETH_DMA_RXConfig
 *
 * @brief			- This function configures Ethernet RX DMA descriptor list
 * 					  address, ring length, tail pointer and RX buffer size
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- RX descriptors must already be initialized before calling
 * 					  this function.
 * 					- ETH_RX_Desc is placed in non-cacheable RAM at 0x24040000.
 *					-
 *
 */
void ETH_DMA_RXConfig(void){
	/*
	 * Rx descriptor ring length register ETH_DMACRXRLR
	 */
	ETH->DMACRXRLR = (ETH_RX_DESC_CNT - 1U);

	/*
	 * Give RX Descriptor list base address to DMA
	 *
	 * DMACRXDLAR = address list of first rx descriptor
	 */
	ETH->DMACRXDLAR = (uint32_t)&ETH_RX_Desc[0];

	/*
	 * RX Descriptor tail pointer
	 */
	ETH->DMACRXDTPR = (uint32_t)&ETH_RX_Desc[ETH_RX_DESC_CNT - 1U];

	/*
	 * RX Buffer Size
	 *
	 * RBSZ field starts at bit 1
	 * ETH_RX_BUF_SIZE = 1536 bytes
	 */
	ETH->DMACRXCR &= ~(ETH_DMACRXCR_RBSZ_MASK);
	ETH->DMACRXCR |= ((uint32_t)ETH_RX_BUF_SIZE << ETH_DMACRXCR_RBSZ);
}

/****************************************************************************
 * @fn				- ETH_DMA_RXStart
 *
 * @brief			- This function starts Ethernet RX DMA
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- RX descriptors must be owned by DMA before starting RX DMA.
 * 					- This sets ETH_DMACRXCR.SR bit.
 *					-
 *
 */
void ETH_DMA_RXStart(void){
	ETH->DMACRXCR |= (1UL << ETH_DMACRXCR_SR);
}

/****************************************************************************
 * @fn				- ETH_ReadRawFrame
 *
 * @brief			- This function checks current RX descriptor and returns
 * 					  received raw Ethernet frame pointer and length if available
 *
 * @param[in]		- pointer to frame pointer variable
 * @param[in]		- pointer to frame length variable
 * @param[in]		-
 *
 * @return			- 1 if a frame is available, 0 if no frame is available
 *
 * @Note			- This is a polling function for early Ethernet RX testing.
 * 					- After reading the frame, user must call
 * 					  ETH_ReleaseRXDescriptor().
 *					-
 *
 */
uint8_t ETH_ReadRawFrame(uint8_t **ppFrame, uint32_t *pLength){
	uint32_t desc3;
	ETH_DMADesc_t *pDesc;

	if((ppFrame == 0U) || (pLength == 0U)) return 0U;

	pDesc = &ETH_RX_Desc[ETH_RX_CurrentDesc];
	desc3 = pDesc->DESC3;

	/*
	 * OWN = 1, DMA still owns descriptor.
	 * No complete frame is available yet.
	 */
	if ((desc3 & ETH_RDES3_OWN) != 0U){
		return 0U;
	}

	/*
	 * OWN = 0, CPU owns descriptor.
	 * If descriptor has error, do not leave it stuck in CPU ownership.
	 */
	if ((desc3 & ETH_RDES3_WB_ES) != 0U){
		ETH_ReleaseRXDescriptor();
		return 0U;
	}

	/*
	 * Optional but recommended:
	 * accept only complete frames.
	 * FD = first descriptor, LD = last descriptor.
	 * For your simple single-buffer RX design, both should be set.
	 */
	if (((desc3 & ETH_RDES3_WB_FD) == 0U) ||
	    ((desc3 & ETH_RDES3_WB_LD) == 0U)){
		ETH_ReleaseRXDescriptor();
		return 0U;
	}

	*pLength = (desc3 & ETH_RDES3_WB_PL_MASK);
	*ppFrame = &ETH_RX_Buffers[ETH_RX_CurrentDesc][0];

	return 1U;
}

/****************************************************************************
 * @fn				- ETH_ReleaseRXDescriptor
 *
 * @brief			- This function gives current RX descriptor back to DMA
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- This function must be called after CPU finishes reading
 * 					  a received frame.
 * 					- It restores descriptor read-format and sets OWN bit.
 *					-
 *
 */
void ETH_ReleaseRXDescriptor(void){
	ETH_DMADesc_t *pDesc;

	pDesc = &ETH_RX_Desc[ETH_RX_CurrentDesc];

	/*
	 * Restore descriptor read-format
	 */
	pDesc->DESC0 = (uint32_t)&ETH_RX_Buffers[ETH_RX_CurrentDesc][0];
	pDesc->DESC1 = 0U;
	pDesc->DESC2 = 0U;
	pDesc->DESC3 = ETH_RDES3_OWN | ETH_RDES3_BUF1V | ETH_RDES3_IOC;

	CPU_DSB();

	/*
	 * Next descriptor
	 */
	ETH_RX_CurrentDesc++;

	if(ETH_RX_CurrentDesc >= ETH_RX_DESC_CNT){
		ETH_RX_CurrentDesc = 0U;
	}

	/*
	 * Notify the DMA that descriptors are available again
	 *
	 * Writing tail pointer is used to resume DMA if it suspended because
	 * no descriptor was available.
	 */
	ETH->DMACRXDTPR = (uint32_t)&ETH_RX_Desc[ETH_RX_DESC_CNT - 1U];

}



/******************************************************************************************************
 * 								PHY / MDIO Register Access
 ******************************************************************************************************/

/****************************************************************************
 * @fn				- ETH_PHY_Read
 *
 * @brief			- This function reads a 16-bit register from an Ethernet PHY
 * 					  using the MDIO/MDC management interface
 *
 * @param[in]		- PHY address on the MDIO bus
 * @param[in]		- PHY register address to read
 * @param[in]		-
 *
 * @return			- 16-bit PHY register value
 *
 * @Note			- For RegAddr selection, see @PHY_REGISTER_ADDRESSES macros.
 * 					- This function uses Clause 22 MDIO access.
 * 					- If 0xFFFF is returned for all registers, PHY address,
 * 					  RMII GPIO configuration or Ethernet clocks should be checked.
 *					-
 *
 */
uint16_t ETH_PHY_Read(uint8_t PHYAddr, uint8_t RegAddr){
	uint32_t tempreg = 0;

	if(ETH_MDIO_WaitReady() == 0U){
		return 0xFFFFU;
	}

	/*
	 * Clause 22 read transaction:
	 *
	 * C45E = 0
	 * PA   = PHY address
	 * RDA  = PHY register address
	 * CR   = MDC clock range
	 * GOC  = Read command
	 * MB   = 1 starts transaction
	 */
	tempreg |= ((uint32_t)(PHYAddr & 0x1FU) << ETH_MACMDIOAR_PA);
	tempreg |= ((uint32_t)(RegAddr & 0x1FU) << ETH_MACMDIOAR_RDA);
	tempreg |= ((uint32_t)(ETH_MDIO_CSR_CLOCK_RANGE & 0x0FU) << ETH_MACMDIOAR_CR);
	tempreg |= ((uint32_t)(ETH_MDIO_GOC_READ & 0x03U) << ETH_MACMDIOAR_GOC);
	tempreg |= (1UL << ETH_MACMDIOAR_MB);

	ETH->MACMDIOAR = tempreg;

	if(ETH_MDIO_WaitReady() == 0U){
		return 0xFFFFU;
	}

	return ((uint16_t)(ETH->MACMDIODR & 0xFFFFU));
}

/****************************************************************************
 * @fn				- ETH_PHY_Write
 *
 * @brief			- This function writes a 16-bit value to an Ethernet PHY register
 * 					  using the MDIO/MDC management interface
 *
 * @param[in]		- PHY address on the MDIO bus
 * @param[in]		- PHY register address to write
 * @param[in]		- 16-bit value to be written
 *
 * @return			- none
 *
 * @Note			- For RegAddr selection, see @PHY_REGISTER_ADDRESSES macros.
 * 					- This function uses Clause 22 MDIO access.
 * 					- It will be used later for PHY reset, auto-negotiation and
 * 					  speed/duplex configuration.
 *					-
 *
 */
void ETH_PHY_Write(uint8_t PHYAddr, uint8_t RegAddr, uint16_t Value){
	uint32_t tempreg = 0;

	if(ETH_MDIO_WaitReady() == 0U){
		return;
	}

	ETH->MACMDIODR = ((uint32_t)Value & 0xFFFFU);

	/*
	 * Clause 22 write transaction
	 */
	tempreg |= ((uint32_t)(PHYAddr & 0x1FU) << ETH_MACMDIOAR_PA);
	tempreg |= ((uint32_t)(RegAddr & 0x1FU) << ETH_MACMDIOAR_RDA);
	tempreg |= ((uint32_t)(ETH_MDIO_CSR_CLOCK_RANGE & 0x0FU) << ETH_MACMDIOAR_CR);
	tempreg |= ((uint32_t)(ETH_MDIO_GOC_WRITE & 0x03U) << ETH_MACMDIOAR_GOC);
	tempreg |= (1UL << ETH_MACMDIOAR_MB);

	ETH->MACMDIOAR = tempreg;

	(void)ETH_MDIO_WaitReady();
}

/******************************************************************************************************
 * 								PHY Status / Link Helper Functions
 ******************************************************************************************************/

/****************************************************************************
 * @fn				- ETH_PHY_FindAddress
 *
 * @brief			- This function scans all possible PHY addresses and finds the
 * 					  address of the connected Ethernet PHY
 *
 * @param[in]		- pointer to variable where found PHY address will be stored
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- 1 if PHY is found, 0 if no PHY is found
 *
 * @Note			- PHY addresses are in the range 0 to 31.
 * 					- The function checks PHY ID1 and PHY ID2 registers.
 *					-
 *
 */
uint8_t ETH_PHY_FindAddress(uint8_t *pPHYAddr){
	uint8_t addr;
	uint16_t id1;
	uint16_t id2;

	if(pPHYAddr == 0) return 0U;

	for(addr = 0U; addr < 32U; addr++){
		id1 = ETH_PHY_Read(addr, PHY_REG_ID1);
		id2 = ETH_PHY_Read(addr, PHY_REG_ID2);

		if((id1 != 0x0000U) && (id1 != 0xFFFFU) &&
		   (id2 != 0x0000U) && (id2 != 0xFFFFU)){
			*pPHYAddr = addr;
			return 1U;
		}
	}

	return 0U;
}

/****************************************************************************
 * @fn				- ETH_PHY_IsLinkUp
 *
 * @brief			- This function checks whether Ethernet link is up or down
 *
 * @param[in]		- PHY address on the MDIO bus
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- 1 if link is up, 0 if link is down
 *
 * @Note			- For link status bit selection, see @PHY_BSR_BITS macros.
 * 					- PHY Basic Status Register link status bit may be latched.
 * 					- Therefore the register is read twice and the second value
 * 					  is used.
 *					-
 *
 */
uint8_t ETH_PHY_IsLinkUp(uint8_t PHYAddr){
	uint16_t bsr;

	/*
	 * First read clears possible latched state
	 */
	bsr = ETH_PHY_Read(PHYAddr, PHY_REG_BSR);

	/*
	 * Second read gives current link status
	 */
	bsr = ETH_PHY_Read(PHYAddr, PHY_REG_BSR);

	if(bsr & PHY_BSR_LINK_STATUS){
		return 1U;
	}

	return 0U;
}

/****************************************************************************
 * @fn				- ETH_PHY_IsAutoNegComplete
 *
 * @brief			- This function checks whether PHY auto-negotiation is completed
 *
 * @param[in]		- PHY address on the MDIO bus
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- 1 if auto-negotiation is completed, 0 if not completed
 *
 * @Note			- For auto-negotiation complete bit, see @PHY_BSR_BITS macros.
 * 					- Auto-negotiation is the process where PHY and link partner
 * 					  agree on speed and duplex mode.
 * 					- This function reads the standard PHY Basic Status Register.
 *					-
 *
 */
uint8_t ETH_PHY_IsAutoNegComplete(uint8_t PHYAddr){
	uint16_t bsr;

	bsr = ETH_PHY_Read(PHYAddr, PHY_REG_BSR);
	bsr = ETH_PHY_Read(PHYAddr, PHY_REG_BSR);

	if((bsr & PHY_BSR_AUTO_NEG_COMPLETE) != 0){
		return 1U;
	}

	return 0;
}

/****************************************************************************
 * @fn				- ETH_PHY_WaitForLink
 *
 * @brief			- This function waits until Ethernet PHY link becomes up
 *
 * @param[in]		- PHY address on the MDIO bus
 * @param[in]		- timeout value for blocking wait loop
 * @param[in]		-
 *
 * @return			- 1 if link becomes up, 0 if timeout occurs
 *
 * @Note			- Link up means that the Ethernet cable is connected and the
 * 					  PHY has established a physical link with the other side.
 * 					- If this function returns 0, check Ethernet cable, router/PC
 * 					  port, PHY power and RJ45 LEDs.
 *					-
 *
 */
uint8_t ETH_PHY_WaitForLink(uint8_t PHYAddr, uint32_t timeout){
	while(ETH_PHY_IsLinkUp(PHYAddr) == 0U){
		if(--timeout == 0U){
			return 0U;
		}
	}

	return 1U;
}

/****************************************************************************
 * @fn				- ETH_PHY_WaitAutoNegotiation
 *
 * @brief			- This function waits until PHY auto-negotiation is completed
 *
 * @param[in]		- PHY address on the MDIO bus
 * @param[in]		- timeout value for blocking wait loop
 * @param[in]		-
 *
 * @return			- 1 if auto-negotiation completes, 0 if timeout occurs
 *
 * @Note			- Auto-negotiation decides speed and duplex between PHY and
 * 					  link partner.
 * 					- For exact speed/duplex result, PHY-specific registers will
 * 					  be read later.
 *					-
 *
 */
uint8_t ETH_PHY_WaitAutoNegotiation(uint8_t PHYAddr, uint32_t timeout){
	while(ETH_PHY_IsAutoNegComplete(PHYAddr) == 0U){
		if(--timeout == 0U){
			return 0U;
		}
	}

	return 1U;
}

/******************************************************************************************************
 * 								Debug Helper Functions
 ******************************************************************************************************/

/****************************************************************************
 * @fn				- ETH_GetRXDescBase
 *
 * @brief			- This function returns RX descriptor base address for debugging
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- pointer to RX descriptor array
 *
 * @Note			- This function is used only to inspect descriptor address
 * 					  in debugger.
 * 					-
 *					-
 *
 */
ETH_DMADesc_t* ETH_GetRXDescBase(void){
	return ETH_RX_Desc;
}

/****************************************************************************
 * @fn				- ETH_GetRXBufferBase
 *
 * @brief			- This function returns RX buffer base address for debugging
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- pointer to first RX buffer byte
 *
 * @Note			- This function is used only to inspect RX buffer address
 * 					  in debugger.
 * 					-
 *					-
 *
 */
uint8_t* ETH_GetRXBufferBase(void){
	return &ETH_RX_Buffers[0][0];
}

/****************************************************************************
 * @fn				- ETH_GetRXDCurrentDescIndex
 *
 * @brief			- This function returns current descriptor index for debugging
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- Index Number of Descriptor
 *
 * @Note			- This function is used only to inspect Descriptor index in debugger.
 * 					-
 *					-
 *
 */
uint32_t ETH_GetRXCurrentDescIndex(void){
	return ETH_RX_CurrentDesc;
}

/****************************************************************************
 * @fn				- ETH_GetRXDesc3
 *
 * @brief			- This function returns RX DESC3 for debugging
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- Value in DECS3/RDES3
 *
 * @Note			- This function is used only to inspect RX DESC3 in debugger.
 * 					-
 *					-
 *
 */
uint32_t ETH_GetRXDesc3(uint32_t desc_index){
	if(desc_index >= ETH_RX_DESC_CNT)
	{
		return 0U;
	}

	return ETH_RX_Desc[desc_index].DESC3;
}

/****************************************************************************
 * @fn				- ETH_MAC_FilterConfigForTest
 *
 * @brief			- This function configures MAC packet filtering for early RX test
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- Promiscuous mode is enabled only for early raw frame testing.
 * 					- Later, this should be replaced with proper MAC filtering.
 *					-
 *
 */
void ETH_MAC_FilterConfigForTest(void){
	/*
	 * PR = 1:
	 * Promiscuous mode. MAC passes all received frames to the application.
	 */
	ETH->MACPFR |= (1UL << ETH_MACPFR_PR);
}

/****************************************************************************
 * @fn				- ETH_MAC_RXEnable
 *
 * @brief			- This function enables Ethernet MAC receiver
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- RX DMA should be started before enabling MAC receiver.
 * 					- This sets ETH_MACCR.RE bit.
 *					-
 *
 */
void ETH_MAC_RXEnable(void){
	/*
	 * Test case:
	 *
	 * FES = 1 -> 100 Mbps
	 * DM  = 1 -> Full duplex
	 * RE  = 1 -> Receiver enable
	 */
	ETH->MACCR |= (1UL << ETH_MACCR_FES);
	ETH->MACCR |= (1UL << ETH_MACCR_DM);
	ETH->MACCR |= (1UL << ETH_MACCR_RE);
	ETH->MACCR |= (1UL << ETH_MACCR_TE);
}

/****************************************************************************
 * @fn				- ETH_MTL_RXQueueConfigRSF
 *
 * @brief			- This function configures Ethernet MTL RX queue
 *
 * @param[in]		- none
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- Store-and-forward mode is used for simpler RX operation.
 * 					- In this mode, MTL stores a complete frame before forwarding
 * 					  it to DMA.
 *					-
 *
 */
void ETH_MTL_RXQueueConfigRSF(void){
	/*
	 * RSF = 1:
	 * Receive store-and-forward mode
	 */
	ETH->MTLRXQOMR |= (1UL << ETH_MTLRXQOMR_RSF);
}
