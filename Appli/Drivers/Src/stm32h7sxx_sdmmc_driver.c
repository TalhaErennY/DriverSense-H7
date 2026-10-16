/*
 * stm32h7sxx_sdmmc_driver.c
 *
 *  Created on: 8 Eki 2026
 *      Author: talha
 */

#include <stddef.h>

#include "stm32h7sxx_sdmmc_driver.h"
#include "stm32h7sxx_gpio_driver.h"


/* -------------------------------------------------------------------------- */
/* Private Macros                                                            */
/* -------------------------------------------------------------------------- */

#define SDMMC_CMD_TIMEOUT					1000000UL

#define SDMMC_CMD_STATIC_FLAGS				\
		((1UL << SDMMC_ICR_CCRCFAILC) |		\
		 (1UL << SDMMC_ICR_CTIMEOUTC) |		\
		 (1UL << SDMMC_ICR_CMDRENDC)   |		\
		 (1UL << SDMMC_ICR_CMDSENTC))


/* -------------------------------------------------------------------------- */
/* Private Function Prototypes                                               */
/* -------------------------------------------------------------------------- */

static void SDMMC_ClearCommandFlags(
		SDMMC_RegDef_t *pSDMMCx);

static uint8_t SDMMC_WaitResponse(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t IgnoreCRC);


/* ========================================================================== */
/* Peripheral Clock Setup                                                     */
/* ========================================================================== */

/****************************************************************************
 * @fn				- SDMMC_PeriClockControl
 *
 * @brief			- This function enables or disables peripheral clock
 *					  for given SDMMC peripheral
 *
 * @param[in]		- base address of SDMMC peripheral
 * @param[in]		- ENABLE or DISABLE macros
 *
 * @return			- none
 *
 * @Note			-
 *
 */
void SDMMC_PeriClockControl(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t ENorDI){

	if(pSDMMCx == SDMMC1){

		if(ENorDI == ENABLE){

			SDMMC1_PCLK_EN();
		}
		else if(ENorDI == DISABLE){

			SDMMC1_PCLK_DI();
		}
	}
}


/* ========================================================================== */
/* Init / DeInit                                                              */
/* ========================================================================== */

/****************************************************************************
 * @fn				- SDMMC_Init
 *
 * @brief			- This function initializes given SDMMC peripheral
 *
 * @param[in]		- pointer to SDMMC handle structure
 *
 * @return			- none
 *
 * @Note			- This function configures SDMMC_CLKCR fields.
 *
 */
void SDMMC_Init(
		SDMMC_Handle_t *pSDMMCHandle){

	uint32_t tempCLKCR = 0U;


	if((pSDMMCHandle == NULL) ||
	   (pSDMMCHandle->pSDMMCx == NULL)){

		return;
	}


	/*
	 * Enable SDMMC peripheral clock.
	 */
	SDMMC_PeriClockControl(
			pSDMMCHandle->pSDMMCx,
			ENABLE);


	/*
	 * Clock divider.
	 *
	 * CLKDIV is 10 bits.
	 */
	tempCLKCR |=
			((pSDMMCHandle->SDMMC_Config.SDMMC_Clockdiv & 0x3FFUL)
			<< SDMMC_CLKCR_CLKDIV);


	/*
	 * Bus width.
	 */
	tempCLKCR |=
			(((uint32_t)pSDMMCHandle->SDMMC_Config.SDMMC_BusWidth & 0x3UL)
			<< SDMMC_CLKCR_WIDBUS);


	/*
	 * Clock edge.
	 */
	tempCLKCR |=
			(((uint32_t)pSDMMCHandle->SDMMC_Config.SDMMC_ClockEdge & 0x1UL)
			<< SDMMC_CLKCR_NEGEDGE);


	/*
	 * Hardware flow control.
	 */
	tempCLKCR |=
			(((uint32_t)pSDMMCHandle->SDMMC_Config.SDMMC_HwFlowControl & 0x1UL)
			<< SDMMC_CLKCR_HWFC_EN);


	/*
	 * Power save.
	 */
	tempCLKCR |=
			(((uint32_t)pSDMMCHandle->SDMMC_Config.SDMMC_PowerSave & 0x1UL)
			<< SDMMC_CLKCR_PWRSAV);


	/*
	 * Write configuration.
	 */
	pSDMMCHandle->pSDMMCx->CLKCR =
			tempCLKCR;
}


/****************************************************************************
 * @fn				- SDMMC_DeInit
 *
 * @brief			- This function resets given SDMMC peripheral
 *
 * @param[in]		- base address of SDMMC peripheral
 *
 * @return			- none
 *
 * @Note			-
 *
 */
void SDMMC_DeInit(
		SDMMC_RegDef_t *pSDMMCx){

	if(pSDMMCx == SDMMC1){

		RCC->AHB5RSTR |=
				(1UL << 8U);

		RCC->AHB5RSTR &=
				~(1UL << 8U);
	}
}


/****************************************************************************
 * @fn				- SDMMC1_GPIOInits
 *
 * @brief			- This function configures SDMMC1 GPIO pins
 *					  for the current 1-bit SD bus connection
 *
 * @param[in]		- none
 *
 * @return			- none
 *
 * @Note			- PC12 -> SDMMC1_CK
 *					  PC8  -> SDMMC1_D0
 *					  PD2  -> SDMMC1_CMD
 *					  AF11
 *
 *					  PB8 is connected to SD DAT3 / old SPI CS.
 *					  In 1-bit native mode it is kept as input pull-up.
 *
 *					  GPIO speed is kept LOW because VHIGH caused
 *					  signal-integrity / CRC problems on the current wiring.
 *
 */
void SDMMC1_GPIOInits(void){

	GPIO_Handle_t SDMMC_Pin = {0};
	GPIO_Handle_t SDMMC_DAT3 = {0};


	/*
	 * Enable GPIO clocks.
	 */
	GPIO_PeriClockControl(GPIOB, ENABLE);
	GPIO_PeriClockControl(GPIOC, ENABLE);
	GPIO_PeriClockControl(GPIOD, ENABLE);


	/*
	 * PB8 -> SD DAT3 / old SPI CS
	 *
	 * Do not drive this pin LOW in native SD mode.
	 */
	SDMMC_DAT3.pGPIOx =
			GPIOB;

	SDMMC_DAT3.GPIO_PinConfig.GPIO_PinNumber =
			GPIO_PIN_NO_8;

	SDMMC_DAT3.GPIO_PinConfig.GPIO_PinMode =
			GPIO_MODE_IN;

	SDMMC_DAT3.GPIO_PinConfig.GPIO_PinPuPdCtrl =
			GPIO_PUPD_PU;

	GPIO_Init(
			&SDMMC_DAT3);


	/*
	 * PC12 -> SDMMC1_CK
	 */
	SDMMC_Pin.pGPIOx =
			GPIOC;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinNumber =
			GPIO_PIN_NO_12;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinMode =
			GPIO_MODE_AF;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinOPType =
			GPIO_OP_TYPE_PP;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinSpeed =
			GPIO_OSPEED_LOW;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinPuPdCtrl =
			GPIO_PUPD_NONE;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinAltFuncMode =
			11U;

	GPIO_Init(
			&SDMMC_Pin);


	/*
	 * PC8 -> SDMMC1_D0
	 */
	SDMMC_Pin.pGPIOx =
			GPIOC;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinNumber =
			GPIO_PIN_NO_8;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinMode =
			GPIO_MODE_AF;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinOPType =
			GPIO_OP_TYPE_PP;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinSpeed =
			GPIO_OSPEED_LOW;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinPuPdCtrl =
			GPIO_PUPD_PU;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinAltFuncMode =
			11U;

	GPIO_Init(
			&SDMMC_Pin);


	/*
	 * PD2 -> SDMMC1_CMD
	 */
	SDMMC_Pin.pGPIOx =
			GPIOD;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinNumber =
			GPIO_PIN_NO_2;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinMode =
			GPIO_MODE_AF;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinOPType =
			GPIO_OP_TYPE_PP;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinSpeed =
			GPIO_OSPEED_LOW;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinPuPdCtrl =
			GPIO_PUPD_PU;

	SDMMC_Pin.GPIO_PinConfig.GPIO_PinAltFuncMode =
			11U;

	GPIO_Init(
			&SDMMC_Pin);
}


/* ========================================================================== */
/* Power / Clock                                                              */
/* ========================================================================== */

/****************************************************************************
 * @fn				- SDMMC_PowerControl
 *
 * @brief			- This function changes SDMMC power state
 *
 * @param[in]		- base address of SDMMC peripheral
 * @param[in]		- @SDMMC_POWER_STATE
 *
 * @return			- none
 *
 * @Note			-
 *
 */
void SDMMC_PowerControl(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t PowerState){

	if(pSDMMCx == NULL){

		return;
	}


	pSDMMCx->POWER &=
			~(0x3UL << SDMMC_POWER_PWRCTRL);

	pSDMMCx->POWER |=
			(((uint32_t)PowerState & 0x3UL)
			<< SDMMC_POWER_PWRCTRL);
}


/****************************************************************************
 * @fn				- SDMMC_SetClockDiv
 *
 * @brief			- This function changes SDMMC CLKDIV field
 *
 * @param[in]		- base address of SDMMC peripheral
 * @param[in]		- 10-bit CLKDIV value
 *
 * @return			- none
 *
 * @Note			- Change the divider only while no command/data transfer
 *					  is active.
 *
 */
void SDMMC_SetClockDiv(
		SDMMC_RegDef_t *pSDMMCx,
		uint16_t ClockDiv){

	if(pSDMMCx == NULL){

		return;
	}


	/*
	 * Do not modify CLKCR while command or data path is active.
	 */
	if((pSDMMCx->STAR &
	   ((1UL << SDMMC_STAR_CPSMACT) |
	    (1UL << SDMMC_STAR_DPSMACT))) != 0U){

		return;
	}


	pSDMMCx->CLKCR &=
			~(0x3FFUL << SDMMC_CLKCR_CLKDIV);

	pSDMMCx->CLKCR |=
			(((uint32_t)ClockDiv & 0x3FFUL)
			<< SDMMC_CLKCR_CLKDIV);
}


/* ========================================================================== */
/* Command Path                                                               */
/* ========================================================================== */

/****************************************************************************
 * @fn				- SDMMC_StartCommand
 *
 * @brief			- This function configures and starts the command
 *					  path state machine (CPSM)
 *
 * @param[in]		- base address of SDMMC peripheral
 * @param[in]		- command index, CMD0 ... CMD63
 * @param[in]		- 32-bit command argument
 * @param[in]		- @SDMMC_RESPONSE_TYPES
 *
 * @return			- @SDMMC_COMMAND_STATUS
 *
 * @Note			- This function only starts the command.
 *					  The response must be handled using the matching
 *					  SDMMC_GetCmdRespX() function.
 *
 */
uint8_t SDMMC_StartCommand(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t CommandIndex,
		uint32_t Argument,
		uint32_t ResponseType){

	uint32_t tempCMDR = 0U;


	if((pSDMMCx == NULL) ||
	   (CommandIndex > 63U)){

		return SDMMC_CMD_ERROR_INVALID_ARG;
	}


	if((ResponseType != SDMMC_RESPONSE_NONE) &&
	   (ResponseType != SDMMC_RESPONSE_SHORT) &&
	   (ResponseType != SDMMC_RESPONSE_SHORT_NO_CRC) &&
	   (ResponseType != SDMMC_RESPONSE_LONG)){

		return SDMMC_CMD_ERROR_INVALID_ARG;
	}


	/*
	 * CPSM must be idle before configuring a new command.
	 */
	if((pSDMMCx->STAR &
	   (1UL << SDMMC_STAR_CPSMACT)) != 0U){

		return SDMMC_CMD_ERROR_BUSY;
	}


	/*
	 * Clear command-related flags from the previous command.
	 */
	SDMMC_ClearCommandFlags(
			pSDMMCx);


	/*
	 * ARGR must be programmed before CMDR.
	 */
	pSDMMCx->ARGR =
			Argument;


	/*
	 * CMDINDEX[5:0].
	 */
	tempCMDR |=
			((uint32_t)(CommandIndex & 0x3FU)
			<< SDMMC_CMDR_CMDINDEX);


	/*
	 * WAITRESP[1:0].
	 *
	 * ResponseType is already aligned to the field.
	 */
	tempCMDR |=
			(ResponseType & SDMMC_RESPONSE_MASK);


	/*
	 * Enable CPSM.
	 */
	tempCMDR |=
			(1UL << SDMMC_CMDR_CPSMEN);


	/*
	 * Writing CMDR starts the command.
	 */
	pSDMMCx->CMDR =
			tempCMDR;


	return SDMMC_CMD_OK;
}


/* ========================================================================== */
/* Response Handling                                                          */
/* ========================================================================== */

/****************************************************************************
 * @fn				- SDMMC_GetCmdNoResponse
 *
 * @brief			- Waits for completion of a command with no response
 *
 * @param[in]		- base address of SDMMC peripheral
 *
 * @return			- @SDMMC_COMMAND_STATUS
 *
 * @Note			- Used by CMD0.
 *
 */
uint8_t SDMMC_GetCmdNoResponse(
		SDMMC_RegDef_t *pSDMMCx){

	uint32_t timeout = SDMMC_CMD_TIMEOUT;
	uint32_t star;


	if(pSDMMCx == NULL){

		return SDMMC_CMD_ERROR_INVALID_ARG;
	}


	do{

		if(timeout-- == 0U){

			return SDMMC_CMD_ERROR_TIMEOUT;
		}


		star =
				pSDMMCx->STAR;

	}while(
		(((star &
		   (1UL << SDMMC_STAR_CMDSENT)) == 0U)

		 ||

		 ((star &
		   (1UL << SDMMC_STAR_CPSMACT)) != 0U))
	);


	pSDMMCx->ICR =
			(1UL << SDMMC_ICR_CMDSENTC);


	return SDMMC_CMD_OK;
}


/****************************************************************************
 * @fn				- SDMMC_GetCmdResp1
 *
 * @brief			- Handles an R1 response
 *
 * @param[in]		- base address of SDMMC peripheral
 * @param[in]		- command index expected in RESPCMDR
 *
 * @return			- @SDMMC_COMMAND_STATUS
 *
 * @Note			- Used by commands such as CMD55, CMD7, CMD13, CMD16.
 *
 */
uint8_t SDMMC_GetCmdResp1(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t CommandIndex){

	uint8_t status;
	uint32_t response;


	if((pSDMMCx == NULL) ||
	   (CommandIndex > 63U)){

		return SDMMC_CMD_ERROR_INVALID_ARG;
	}


	status =
			SDMMC_WaitResponse(
					pSDMMCx,
					0U);


	if(status != SDMMC_CMD_OK){

		return status;
	}


	if((pSDMMCx->RESPCMDR & 0x3FU) !=
	   (CommandIndex & 0x3FU)){

		return SDMMC_CMD_ERROR_RESPCMD;
	}


	response =
			pSDMMCx->RESP1R;


	if((response &
		SDMMC_R1_ERROR_BITS) != 0U){

		return SDMMC_CMD_ERROR_CARD;
	}


	return SDMMC_CMD_OK;
}


/****************************************************************************
 * @fn				- SDMMC_GetCmdResp2
 *
 * @brief			- Handles an R2 long response
 *
 * @param[in]		- base address of SDMMC peripheral
 *
 * @return			- @SDMMC_COMMAND_STATUS
 *
 * @Note			- Used by CMD2 (CID) and CMD9 (CSD).
 *
 */
uint8_t SDMMC_GetCmdResp2(
		SDMMC_RegDef_t *pSDMMCx){

	if(pSDMMCx == NULL){

		return SDMMC_CMD_ERROR_INVALID_ARG;
	}


	return SDMMC_WaitResponse(
			pSDMMCx,
			0U);
}


/****************************************************************************
 * @fn				- SDMMC_GetCmdResp3
 *
 * @brief			- Handles an R3 response
 *
 * @param[in]		- base address of SDMMC peripheral
 *
 * @return			- @SDMMC_COMMAND_STATUS
 *
 * @Note			- Used by ACMD41.
 *
 *					  R3 has no valid CRC7 field. Therefore CCRCFAIL is
 *					  intentionally ignored here.
 *
 */
uint8_t SDMMC_GetCmdResp3(
		SDMMC_RegDef_t *pSDMMCx){

	if(pSDMMCx == NULL){

		return SDMMC_CMD_ERROR_INVALID_ARG;
	}


	return SDMMC_WaitResponse(
			pSDMMCx,
			1U);
}


/****************************************************************************
 * @fn				- SDMMC_GetCmdResp6
 *
 * @brief			- Handles an R6 response and extracts RCA
 *
 * @param[in]		- base address of SDMMC peripheral
 * @param[in]		- command index expected in RESPCMDR
 * @param[out]		- pointer to 16-bit RCA
 *
 * @return			- @SDMMC_COMMAND_STATUS
 *
 * @Note			- Used by CMD3.
 *
 */
uint8_t SDMMC_GetCmdResp6(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t CommandIndex,
		uint16_t *pRCA){

	uint8_t status;
	uint32_t response;


	if((pSDMMCx == NULL) ||
	   (pRCA == NULL) ||
	   (CommandIndex > 63U)){

		return SDMMC_CMD_ERROR_INVALID_ARG;
	}


	status =
			SDMMC_WaitResponse(
					pSDMMCx,
					0U);


	if(status != SDMMC_CMD_OK){

		return status;
	}


	if((pSDMMCx->RESPCMDR & 0x3FU) !=
	   (CommandIndex & 0x3FU)){

		return SDMMC_CMD_ERROR_RESPCMD;
	}


	response =
			pSDMMCx->RESP1R;


	/*
	 * R6 bits [31:16] contain the Relative Card Address.
	 */
	*pRCA =
			(uint16_t)(response >> 16U);


	/*
	 * R6 lower-half error bits:
	 *
	 * bit15 -> COM_CRC_ERROR
	 * bit14 -> ILLEGAL_COMMAND
	 * bit13 -> ERROR
	 */
	if((response &
		SDMMC_R6_ERROR_BITS) != 0U){

		return SDMMC_CMD_ERROR_CARD;
	}


	return SDMMC_CMD_OK;
}


/****************************************************************************
 * @fn				- SDMMC_GetCmdResp7
 *
 * @brief			- Handles an R7 response
 *
 * @param[in]		- base address of SDMMC peripheral
 * @param[in]		- command index expected in RESPCMDR
 *
 * @return			- @SDMMC_COMMAND_STATUS
 *
 * @Note			- Used by CMD8.
 *
 */
uint8_t SDMMC_GetCmdResp7(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t CommandIndex){

	uint8_t status;


	if((pSDMMCx == NULL) ||
	   (CommandIndex > 63U)){

		return SDMMC_CMD_ERROR_INVALID_ARG;
	}


	status =
			SDMMC_WaitResponse(
					pSDMMCx,
					0U);


	if(status != SDMMC_CMD_OK){

		return status;
	}


	if((pSDMMCx->RESPCMDR & 0x3FU) !=
	   (CommandIndex & 0x3FU)){

		return SDMMC_CMD_ERROR_RESPCMD;
	}


	return SDMMC_CMD_OK;
}


/****************************************************************************
 * @fn				- SDMMC_WaitBusyD0Release
 *
 * @brief			- Waits until card releases DAT0 busy signal
 *
 * @param[in]		- base address of SDMMC peripheral
 *
 * @return			- @SDMMC_COMMAND_STATUS
 *
 * @Note			- Used after R1b commands such as CMD7.
 *
 */
uint8_t SDMMC_WaitBusyD0Release(
		SDMMC_RegDef_t *pSDMMCx){

	uint32_t timeout = SDMMC_CMD_TIMEOUT;


	if(pSDMMCx == NULL){

		return SDMMC_CMD_ERROR_INVALID_ARG;
	}


	while((pSDMMCx->STAR &
		  (1UL << SDMMC_STAR_BUSYD0)) != 0U){

		if(timeout-- == 0U){

			return SDMMC_CMD_ERROR_TIMEOUT;
		}
	}


	/*
	 * Clear BUSYD0END if it was generated.
	 */
	if((pSDMMCx->STAR &
	   (1UL << SDMMC_STAR_BUSYD0END)) != 0U){

		pSDMMCx->ICR =
				(1UL << SDMMC_ICR_BUSYD0ENDC);
	}


	return SDMMC_CMD_OK;
}


/****************************************************************************
 * @fn				- SDMMC_GetResponse
 *
 * @brief			- Returns one SDMMC response register
 *
 * @param[in]		- base address of SDMMC peripheral
 * @param[in]		- @SDMMC_RESPONSE_REGISTERS
 *
 * @return			- selected response register value
 *
 * @Note			-
 *
 */
uint32_t SDMMC_GetResponse(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t ResponseRegister){

	if(pSDMMCx == NULL){

		return 0U;
	}


	switch(ResponseRegister){

	case SDMMC_RESPONSE_REG_1:

		return pSDMMCx->RESP1R;


	case SDMMC_RESPONSE_REG_2:

		return pSDMMCx->RESP2R;


	case SDMMC_RESPONSE_REG_3:

		return pSDMMCx->RESP3R;


	case SDMMC_RESPONSE_REG_4:

		return pSDMMCx->RESP4R;


	default:

		return 0U;
	}
}


/* ========================================================================== */
/* Compatibility Helpers                                                      */
/* ========================================================================== */

/****************************************************************************
 * @fn				- SDMMC_SendCommand
 *
 * @brief			- Compatibility helper for the older test code
 *
 * @Note			- New code should preferably use:
 *
 *					  SDMMC_StartCommand()
 *					  +
 *					  SDMMC_GetCmdRespX()
 *
 */
uint8_t SDMMC_SendCommand(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t CommandIndex,
		uint32_t Argument,
		uint32_t ResponseType){

	uint8_t status;


	status =
			SDMMC_StartCommand(
					pSDMMCx,
					CommandIndex,
					Argument,
					ResponseType);


	if(status != SDMMC_CMD_OK){

		return status;
	}


	if(ResponseType == SDMMC_RESPONSE_NONE){

		return SDMMC_GetCmdNoResponse(
				pSDMMCx);
	}


	if(ResponseType == SDMMC_RESPONSE_LONG){

		return SDMMC_GetCmdResp2(
				pSDMMCx);
	}


	if(ResponseType == SDMMC_RESPONSE_SHORT_NO_CRC){

		return SDMMC_WaitResponse(
				pSDMMCx,
				1U);
	}


	/*
	 * Generic short response:
	 *
	 * Check transport-level CRC/timeout and mirrored command index,
	 * but do not interpret RESP1R as R1 because it may be R6/R7.
	 */
	status =
			SDMMC_WaitResponse(
					pSDMMCx,
					0U);


	if(status != SDMMC_CMD_OK){

		return status;
	}


	if((pSDMMCx->RESPCMDR & 0x3FU) !=
	   (CommandIndex & 0x3FU)){

		return SDMMC_CMD_ERROR_RESPCMD;
	}


	return SDMMC_CMD_OK;
}


/****************************************************************************
 * @fn				- SDMMC_SendCommandR3
 *
 * @brief			- Compatibility helper for ACMD41 / R3
 *
 * @Note			- R3 is started as a normal SHORT response and
 *					  CCRCFAIL is ignored by SDMMC_GetCmdResp3().
 *
 */
uint8_t SDMMC_SendCommandR3(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t CommandIndex,
		uint32_t Argument){

	uint8_t status;


	status =
			SDMMC_StartCommand(
					pSDMMCx,
					CommandIndex,
					Argument,
					SDMMC_RESPONSE_SHORT);


	if(status != SDMMC_CMD_OK){

		return status;
	}


	return SDMMC_GetCmdResp3(
			pSDMMCx);
}


/* ========================================================================== */
/* Private Functions                                                          */
/* ========================================================================== */

/****************************************************************************
 * @fn				- SDMMC_ClearCommandFlags
 *
 * @brief			- Clears command-related status flags
 *
 */
static void SDMMC_ClearCommandFlags(
		SDMMC_RegDef_t *pSDMMCx){

	pSDMMCx->ICR =
			SDMMC_CMD_STATIC_FLAGS;
}


/****************************************************************************
 * @fn				- SDMMC_WaitResponse
 *
 * @brief			- Waits until command response path finishes
 *
 * @param[in]		- base address of SDMMC peripheral
 * @param[in]		- 0: CRC error is treated as error
 *					  1: CRC error is ignored (R3)
 *
 * @return			- @SDMMC_COMMAND_STATUS
 *
 */
static uint8_t SDMMC_WaitResponse(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t IgnoreCRC){

	uint32_t timeout = SDMMC_CMD_TIMEOUT;
	uint32_t star;


	if(pSDMMCx == NULL){

		return SDMMC_CMD_ERROR_INVALID_ARG;
	}


	do{

		if(timeout-- == 0U){

			return SDMMC_CMD_ERROR_TIMEOUT;
		}


		star =
				pSDMMCx->STAR;

	}while(
		(((star &
		  ((1UL << SDMMC_STAR_CCRCFAIL) |
		   (1UL << SDMMC_STAR_CMDREND)  |
		   (1UL << SDMMC_STAR_CTIMEOUT))) == 0U)

		 ||

		 ((star &
		   (1UL << SDMMC_STAR_CPSMACT)) != 0U))
	);


	/*
	 * No response arrived before the hardware timeout.
	 */
	if((star &
	   (1UL << SDMMC_STAR_CTIMEOUT)) != 0U){

		SDMMC_ClearCommandFlags(
				pSDMMCx);

		return SDMMC_CMD_ERROR_TIMEOUT;
	}


	/*
	 * For R1/R2/R6/R7 this is a real transport CRC error.
	 *
	 * For R3 it is ignored because R3 does not carry a valid CRC7.
	 */
	if((IgnoreCRC == 0U) &&
	   ((star &
	    (1UL << SDMMC_STAR_CCRCFAIL)) != 0U)){

		SDMMC_ClearCommandFlags(
				pSDMMCx);

		return SDMMC_CMD_ERROR_CRC;
	}


	/*
	 * Clear command flags after the response has been captured.
	 *
	 * RESP1R..RESP4R and RESPCMDR keep the received response data.
	 */
	SDMMC_ClearCommandFlags(
			pSDMMCx);


	return SDMMC_CMD_OK;
}
