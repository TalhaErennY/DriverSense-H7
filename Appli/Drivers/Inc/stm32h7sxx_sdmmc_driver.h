/*
 * stm32h7sxx_sdmmc_driver.h
 *
 *  Created on: 8 Eki 2026
 *      Author: talha
 */

#ifndef INC_STM32H7SXX_SDMMC_DRIVER_H_
#define INC_STM32H7SXX_SDMMC_DRIVER_H_

#include <stdint.h>
#include "stm32h7sxx.h"


/* -------------------------------------------------------------------------- */
/* SDMMC Configuration Structures                                             */
/* -------------------------------------------------------------------------- */

typedef struct{
	uint32_t SDMMC_Clockdiv;
	uint8_t  SDMMC_BusWidth;
	uint8_t  SDMMC_ClockEdge;
	uint8_t  SDMMC_HwFlowControl;
	uint8_t  SDMMC_PowerSave;
} SDMMC_Config_t;


typedef struct{
	SDMMC_RegDef_t *pSDMMCx;
	SDMMC_Config_t SDMMC_Config;
} SDMMC_Handle_t;


/* -------------------------------------------------------------------------- */
/* @SDMMC_BUS_WIDTH                                                           */
/* -------------------------------------------------------------------------- */

#define SDMMC_BUS_WIDTH_1BIT				0U
#define SDMMC_BUS_WIDTH_4BIT				1U
#define SDMMC_BUS_WIDTH_8BIT				2U


/* -------------------------------------------------------------------------- */
/* @SDMMC_CLOCK_EDGE                                                         */
/* -------------------------------------------------------------------------- */

#define SDMMC_CLOCK_EDGE_RISING				0U
#define SDMMC_CLOCK_EDGE_FALLING			1U


/* -------------------------------------------------------------------------- */
/* @SDMMC_POWER_SAVE                                                         */
/* -------------------------------------------------------------------------- */

#define SDMMC_POWER_SAVE_DISABLE			0U
#define SDMMC_POWER_SAVE_ENABLE				1U


/* -------------------------------------------------------------------------- */
/* @SDMMC_POWER_STATE                                                        */
/* -------------------------------------------------------------------------- */

#define SDMMC_POWER_OFF						0U
#define SDMMC_POWER_CYCLE					2U
#define SDMMC_POWER_ON						3U


/* -------------------------------------------------------------------------- */
/* @SDMMC_HW_FLOW_CONTROL                                                    */
/* -------------------------------------------------------------------------- */

#define SDMMC_HW_FLOW_DISABLE				0U
#define SDMMC_HW_FLOW_ENABLE					1U


/* -------------------------------------------------------------------------- */
/* @SDMMC_RESPONSE_TYPES                                                     */
/*
 * These values are already aligned to CMDR.WAITRESP[1:0].
 *
 * WAITRESP = 00 -> no response
 * WAITRESP = 01 -> short response
 * WAITRESP = 10 -> short response, no CRC check
 * WAITRESP = 11 -> long response
 *
 * For SD memory-card R3 (ACMD41), this driver follows the STM32H7RS-style
 * flow: command is started with SDMMC_RESPONSE_SHORT and the CRC result is
 * ignored by SDMMC_GetCmdResp3().
 */
/* -------------------------------------------------------------------------- */

#define SDMMC_RESPONSE_NONE					(0x0UL << SDMMC_CMDR_WAITRESP)
#define SDMMC_RESPONSE_SHORT					(0x1UL << SDMMC_CMDR_WAITRESP)
#define SDMMC_RESPONSE_SHORT_NO_CRC			(0x2UL << SDMMC_CMDR_WAITRESP)
#define SDMMC_RESPONSE_LONG					(0x3UL << SDMMC_CMDR_WAITRESP)

#define SDMMC_RESPONSE_MASK					(0x3UL << SDMMC_CMDR_WAITRESP)


/* -------------------------------------------------------------------------- */
/* @SDMMC_COMMAND_STATUS                                                     */
/* -------------------------------------------------------------------------- */

#define SDMMC_CMD_OK							0U
#define SDMMC_CMD_ERROR_TIMEOUT				1U
#define SDMMC_CMD_ERROR_CRC					2U
#define SDMMC_CMD_ERROR_BUSY					3U
#define SDMMC_CMD_ERROR_INVALID_ARG			4U
#define SDMMC_CMD_ERROR_RESPCMD				5U
#define SDMMC_CMD_ERROR_CARD					6U


/* -------------------------------------------------------------------------- */
/* @SDMMC_RESPONSE_REGISTERS                                                 */
/* -------------------------------------------------------------------------- */

#define SDMMC_RESPONSE_REG_1					1U
#define SDMMC_RESPONSE_REG_2					2U
#define SDMMC_RESPONSE_REG_3					3U
#define SDMMC_RESPONSE_REG_4					4U


/* -------------------------------------------------------------------------- */
/* Card-response masks                                                       */
/* -------------------------------------------------------------------------- */

/*
 * Error bits present in an SD card R1 response.
 */
#define SDMMC_R1_ERROR_BITS					0xFDFFE008UL

/*
 * Error bits present in the lower 16 bits of an R6 response.
 */
#define SDMMC_R6_ERROR_BITS					0x0000E000UL


/* -------------------------------------------------------------------------- */
/* Peripheral Clock Setup                                                    */
/* -------------------------------------------------------------------------- */

void SDMMC_PeriClockControl(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t ENorDI);


/* -------------------------------------------------------------------------- */
/* Init / DeInit                                                             */
/* -------------------------------------------------------------------------- */

void SDMMC_Init(
		SDMMC_Handle_t *pSDMMCHandle);

void SDMMC_DeInit(
		SDMMC_RegDef_t *pSDMMCx);

void SDMMC1_GPIOInits(void);


/* -------------------------------------------------------------------------- */
/* Power / Clock                                                             */
/* -------------------------------------------------------------------------- */

void SDMMC_PowerControl(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t PowerState);

void SDMMC_SetClockDiv(
		SDMMC_RegDef_t *pSDMMCx,
		uint16_t ClockDiv);


/* -------------------------------------------------------------------------- */
/* Command Path                                                              */
/* -------------------------------------------------------------------------- */

uint8_t SDMMC_StartCommand(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t CommandIndex,
		uint32_t Argument,
		uint32_t ResponseType);


/* -------------------------------------------------------------------------- */
/* Response Handling                                                         */
/* -------------------------------------------------------------------------- */

uint8_t SDMMC_GetCmdNoResponse(
		SDMMC_RegDef_t *pSDMMCx);

uint8_t SDMMC_GetCmdResp1(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t CommandIndex);

uint8_t SDMMC_GetCmdResp2(
		SDMMC_RegDef_t *pSDMMCx);

uint8_t SDMMC_GetCmdResp3(
		SDMMC_RegDef_t *pSDMMCx);

uint8_t SDMMC_GetCmdResp6(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t CommandIndex,
		uint16_t *pRCA);

uint8_t SDMMC_GetCmdResp7(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t CommandIndex);

uint8_t SDMMC_WaitBusyD0Release(
		SDMMC_RegDef_t *pSDMMCx);

uint32_t SDMMC_GetResponse(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t ResponseRegister);


/* -------------------------------------------------------------------------- */
/* Compatibility Helpers                                                     */
/*
 * These keep the older test code usable while the project is migrated to
 * SDMMC_StartCommand() + SDMMC_GetCmdRespX().
 */
/* -------------------------------------------------------------------------- */

uint8_t SDMMC_SendCommand(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t CommandIndex,
		uint32_t Argument,
		uint32_t ResponseType);

uint8_t SDMMC_SendCommandR3(
		SDMMC_RegDef_t *pSDMMCx,
		uint8_t CommandIndex,
		uint32_t Argument);


#endif /* INC_STM32H7SXX_SDMMC_DRIVER_H_ */
