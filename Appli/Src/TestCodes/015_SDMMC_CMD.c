/*
 * 015_SDMMC_CMD.c
 *
 *  Created on: 8 Eki 2026
 *      Author: talha
 */

#include <stdint.h>

#include "stm32h7sxx.h"
#include "stm32h7sxx_helpers.h"
#include "stm32h7sxx_rcc_driver.h"
#include "stm32h7sxx_sdmmc_driver.h"


/* -------------------------------------------------------------------------- */
/* SD Commands                                                                */
/* -------------------------------------------------------------------------- */

#define SD_CMD0                     0U
#define SD_CMD2                     2U
#define SD_CMD3                     3U
#define SD_CMD7                     7U
#define SD_CMD8                     8U
#define SD_CMD9                     9U
#define SD_CMD13                    13U
#define SD_CMD16                    16U
#define SD_CMD55                    55U

#define SD_ACMD41                   41U


/* -------------------------------------------------------------------------- */
/* ACMD41                                                                     */
/* -------------------------------------------------------------------------- */

#define SD_ACMD41_ARG               0x40FF8000UL

#define SD_OCR_BUSY                 (1UL << 31U)
#define SD_OCR_CCS                  (1UL << 30U)
#define SD_OCR_VOLTAGE_MASK         0x00FF8000UL

#define SD_ACMD41_MAX_ATTEMPTS      1000U


/* -------------------------------------------------------------------------- */
/* R1 Status                                                                  */
/* -------------------------------------------------------------------------- */

#define SD_R1_READY_FOR_DATA        (1UL << 8U)

#define SD_R1_CURRENT_STATE         9U
#define SD_R1_CURRENT_STATE_MASK    (0xFUL << SD_R1_CURRENT_STATE)

#define SD_R1_APP_CMD               (1UL << 5U)


/*
 * SD Card State values.
 */
#define SD_CARD_STATE_IDLE          0U
#define SD_CARD_STATE_READY         1U
#define SD_CARD_STATE_IDENT         2U
#define SD_CARD_STATE_STBY          3U
#define SD_CARD_STATE_TRAN          4U


/* -------------------------------------------------------------------------- */
/* Test Steps                                                                 */
/* -------------------------------------------------------------------------- */

#define SD_TEST_STEP_START          0U
#define SD_TEST_STEP_CMD0           1U
#define SD_TEST_STEP_CMD8           2U
#define SD_TEST_STEP_ACMD41         3U
#define SD_TEST_STEP_CMD2           4U
#define SD_TEST_STEP_CMD3           5U
#define SD_TEST_STEP_CMD9           6U
#define SD_TEST_STEP_CMD7           7U
#define SD_TEST_STEP_CMD16          8U
#define SD_TEST_STEP_CMD13          9U
#define SD_TEST_STEP_DONE           10U


/* -------------------------------------------------------------------------- */
/* General Debug                                                              */
/* -------------------------------------------------------------------------- */

volatile uint8_t dbg_test_step = SD_TEST_STEP_START;
volatile uint8_t dbg_test_pass = 0U;
volatile uint8_t dbg_test_error = 0U;


/* -------------------------------------------------------------------------- */
/* CMD0                                                                       */
/* -------------------------------------------------------------------------- */

volatile uint8_t dbg_cmd0_status = 0xFFU;


/* -------------------------------------------------------------------------- */
/* CMD8                                                                       */
/* -------------------------------------------------------------------------- */

volatile uint8_t  dbg_cmd8_status = 0xFFU;
volatile uint32_t dbg_cmd8_response = 0U;
volatile uint32_t dbg_cmd8_respcmd = 0U;
volatile uint8_t  dbg_cmd8_pattern_ok = 0U;


/* -------------------------------------------------------------------------- */
/* CMD55 + ACMD41                                                             */
/* -------------------------------------------------------------------------- */

volatile uint8_t  dbg_cmd55_status = 0xFFU;
volatile uint32_t dbg_cmd55_response = 0U;

volatile uint8_t  dbg_acmd41_status = 0xFFU;
volatile uint32_t dbg_acmd41_ocr = 0U;
volatile uint16_t dbg_acmd41_attempt = 0U;

volatile uint8_t dbg_card_ready = 0U;
volatile uint8_t dbg_card_high_capacity = 0U;


/* -------------------------------------------------------------------------- */
/* CMD2 - CID                                                                 */
/* -------------------------------------------------------------------------- */

volatile uint8_t dbg_cmd2_status = 0xFFU;

volatile uint32_t dbg_cid[4] = {
		0U, 0U, 0U, 0U
};


/* -------------------------------------------------------------------------- */
/* CMD3 - RCA                                                                 */
/* -------------------------------------------------------------------------- */

volatile uint8_t  dbg_cmd3_status = 0xFFU;
volatile uint32_t dbg_cmd3_response = 0U;
volatile uint16_t dbg_card_rca = 0U;


/* -------------------------------------------------------------------------- */
/* CMD9 - CSD                                                                 */
/* -------------------------------------------------------------------------- */

volatile uint8_t dbg_cmd9_status = 0xFFU;

volatile uint32_t dbg_csd[4] = {
		0U, 0U, 0U, 0U
};


/* -------------------------------------------------------------------------- */
/* CMD7                                                                       */
/* -------------------------------------------------------------------------- */

volatile uint8_t dbg_cmd7_status = 0xFFU;
volatile uint8_t dbg_cmd7_busy_status = 0xFFU;


/* -------------------------------------------------------------------------- */
/* CMD16                                                                      */
/* -------------------------------------------------------------------------- */

volatile uint8_t dbg_cmd16_status = 0xFFU;


/* -------------------------------------------------------------------------- */
/* CMD13                                                                      */
/* -------------------------------------------------------------------------- */

volatile uint8_t  dbg_cmd13_status = 0xFFU;
volatile uint32_t dbg_card_status = 0U;

volatile uint8_t dbg_card_current_state = 0xFFU;
volatile uint8_t dbg_card_ready_for_data = 0U;


/* -------------------------------------------------------------------------- */
/* SDMMC                                                                      */
/* -------------------------------------------------------------------------- */

volatile uint32_t dbg_sdmmc_power = 0U;
volatile uint32_t dbg_sdmmc_clkcr = 0U;
volatile uint32_t dbg_sdmmc_cmdr = 0U;
volatile uint32_t dbg_sdmmc_star = 0U;


/* -------------------------------------------------------------------------- */
/* Test                                                                       */
/* -------------------------------------------------------------------------- */

void Test_015_SDMMC_CMD(void){

	SDMMC_Handle_t SDMMC1Handle = {0};
	uint8_t status;


	/* ====================================================================== */
	/* Hardware Initialization                                                */
	/* ====================================================================== */

	RCC_SystemClock_Config_280MHz_HSI();

	RCC_SDMMC_ClockConfig_50MHz();

	SDMMC1_GPIOInits();


	/*
	 * SDMMC identification clock:
	 *
	 * 50 MHz / (2 * 64)
	 *
	 * = 390.625 kHz
	 */
	SDMMC1Handle.pSDMMCx = SDMMC1;

	SDMMC1Handle.SDMMC_Config.SDMMC_Clockdiv =
			64U;

	SDMMC1Handle.SDMMC_Config.SDMMC_BusWidth =
			SDMMC_BUS_WIDTH_1BIT;

	SDMMC1Handle.SDMMC_Config.SDMMC_ClockEdge =
			SDMMC_CLOCK_EDGE_RISING;

	SDMMC1Handle.SDMMC_Config.SDMMC_HwFlowControl =
			SDMMC_HW_FLOW_DISABLE;

	SDMMC1Handle.SDMMC_Config.SDMMC_PowerSave =
			SDMMC_POWER_SAVE_DISABLE;


	SDMMC_Init(&SDMMC1Handle);

	SDMMC_PowerControl(
			SDMMC1,
			SDMMC_POWER_ON);


	dbg_sdmmc_power = SDMMC1->POWER;
	dbg_sdmmc_clkcr = SDMMC1->CLKCR;


	BlockingDelay_ms(50U);


	/* ====================================================================== */
	/* CMD0 - GO_IDLE_STATE                                                   */
	/* ====================================================================== */

	dbg_test_step = SD_TEST_STEP_CMD0;


	status =
			SDMMC_StartCommand(
					SDMMC1,
					SD_CMD0,
					0x00000000UL,
					SDMMC_RESPONSE_NONE);


	if(status != SDMMC_CMD_OK){

		dbg_test_error = status;
		goto test_end;
	}


	dbg_cmd0_status =
			SDMMC_GetCmdNoResponse(
					SDMMC1);


	if(dbg_cmd0_status != SDMMC_CMD_OK){

		dbg_test_error = dbg_cmd0_status;
		goto test_end;
	}


	BlockingDelay_ms(2U);


	/* ====================================================================== */
	/* CMD8 - SEND_IF_COND                                                    */
	/* ====================================================================== */

	dbg_test_step = SD_TEST_STEP_CMD8;


	status =
			SDMMC_StartCommand(
					SDMMC1,
					SD_CMD8,
					0x000001AAUL,
					SDMMC_RESPONSE_SHORT);


	if(status != SDMMC_CMD_OK){

		dbg_test_error = status;
		goto test_end;
	}


	dbg_cmd8_status =
			SDMMC_GetCmdResp7(
					SDMMC1,
					SD_CMD8);


	if(dbg_cmd8_status != SDMMC_CMD_OK){

		dbg_test_error = dbg_cmd8_status;
		goto test_end;
	}


	dbg_cmd8_response =
			SDMMC1->RESP1R;

	dbg_cmd8_respcmd =
			SDMMC1->RESPCMDR;


	/*
	 * Voltage Accepted = 1
	 * Check Pattern    = 0xAA
	 */
	if((dbg_cmd8_response & 0xFFFU) !=
	   0x1AAU){

		dbg_test_error =
				SDMMC_CMD_ERROR_CARD;

		goto test_end;
	}


	dbg_cmd8_pattern_ok = 1U;


	/* ====================================================================== */
	/* CMD55 + ACMD41                                                         */
	/* ====================================================================== */

	dbg_test_step = SD_TEST_STEP_ACMD41;


	for(uint16_t attempt = 0U;
		attempt < SD_ACMD41_MAX_ATTEMPTS;
		attempt++){

		dbg_acmd41_attempt =
				attempt + 1U;


		/* ------------------------------------------------------------------ */
		/* CMD55 - APP_CMD                                                    */
		/* ------------------------------------------------------------------ */

		status =
				SDMMC_StartCommand(
						SDMMC1,
						SD_CMD55,
						0x00000000UL,
						SDMMC_RESPONSE_SHORT);


		if(status != SDMMC_CMD_OK){

			dbg_test_error = status;
			goto test_end;
		}


		dbg_cmd55_status =
				SDMMC_GetCmdResp1(
						SDMMC1,
						SD_CMD55);


		if(dbg_cmd55_status !=
		   SDMMC_CMD_OK){

			dbg_test_error =
					dbg_cmd55_status;

			goto test_end;
		}


		dbg_cmd55_response =
				SDMMC1->RESP1R;


		/*
		 * Card must accept APP_CMD.
		 */
		if((dbg_cmd55_response &
			SD_R1_APP_CMD) == 0U){

			dbg_test_error =
					SDMMC_CMD_ERROR_CARD;

			goto test_end;
		}


		/* ------------------------------------------------------------------ */
		/* ACMD41 - SD_SEND_OP_COND                                           */
		/* ------------------------------------------------------------------ */

		/*
		 * Important:
		 *
		 * ST H7RS LL driver sends ACMD41 as SHORT response
		 * and handles it using an R3-specific response parser.
		 */
		status =
				SDMMC_StartCommand(
						SDMMC1,
						SD_ACMD41,
						SD_ACMD41_ARG,
						SDMMC_RESPONSE_SHORT);


		if(status != SDMMC_CMD_OK){

			dbg_test_error = status;
			goto test_end;
		}


		dbg_acmd41_status =
				SDMMC_GetCmdResp3(
						SDMMC1);


		if(dbg_acmd41_status !=
		   SDMMC_CMD_OK){

			dbg_test_error =
					dbg_acmd41_status;

			goto test_end;
		}


		dbg_acmd41_ocr =
				SDMMC1->RESP1R;


		/*
		 * Card must advertise at least one voltage
		 * in our requested range.
		 */
		if((dbg_acmd41_ocr &
			SD_OCR_VOLTAGE_MASK) == 0U){

			BlockingDelay_ms(1U);

			continue;
		}


		/*
		 * Bit31:
		 *
		 * 0 -> still powering up
		 * 1 -> initialization finished
		 */
		if((dbg_acmd41_ocr &
			SD_OCR_BUSY) != 0U){

			dbg_card_ready = 1U;

			break;
		}


		BlockingDelay_ms(1U);
	}


	if(dbg_card_ready == 0U){

		dbg_test_error =
				SDMMC_CMD_ERROR_TIMEOUT;

		goto test_end;
	}


	/*
	 * OCR CCS bit.
	 */
	if((dbg_acmd41_ocr &
		SD_OCR_CCS) != 0U){

		dbg_card_high_capacity = 1U;
	}
	else{

		dbg_card_high_capacity = 0U;
	}


	/* ====================================================================== */
	/* CMD2 - ALL_SEND_CID                                                    */
	/* ====================================================================== */

	dbg_test_step = SD_TEST_STEP_CMD2;


	status =
			SDMMC_StartCommand(
					SDMMC1,
					SD_CMD2,
					0x00000000UL,
					SDMMC_RESPONSE_LONG);


	if(status != SDMMC_CMD_OK){

		dbg_test_error = status;
		goto test_end;
	}


	dbg_cmd2_status =
			SDMMC_GetCmdResp2(
					SDMMC1);


	if(dbg_cmd2_status !=
	   SDMMC_CMD_OK){

		dbg_test_error =
				dbg_cmd2_status;

		goto test_end;
	}


	dbg_cid[0] = SDMMC1->RESP1R;
	dbg_cid[1] = SDMMC1->RESP2R;
	dbg_cid[2] = SDMMC1->RESP3R;
	dbg_cid[3] = SDMMC1->RESP4R;


	/* ====================================================================== */
	/* CMD3 - SEND_RELATIVE_ADDR                                              */
	/* ====================================================================== */

	dbg_test_step = SD_TEST_STEP_CMD3;


	status =
			SDMMC_StartCommand(
					SDMMC1,
					SD_CMD3,
					0x00000000UL,
					SDMMC_RESPONSE_SHORT);


	if(status != SDMMC_CMD_OK){

		dbg_test_error = status;
		goto test_end;
	}


	dbg_cmd3_status =
			SDMMC_GetCmdResp6(
					SDMMC1,
					SD_CMD3,
					(uint16_t *)&dbg_card_rca);


	if(dbg_cmd3_status !=
	   SDMMC_CMD_OK){

		dbg_test_error =
				dbg_cmd3_status;

		goto test_end;
	}


	dbg_cmd3_response =
			SDMMC1->RESP1R;


	if(dbg_card_rca == 0U){

		dbg_test_error =
				SDMMC_CMD_ERROR_CARD;

		goto test_end;
	}


	/* ====================================================================== */
	/* CMD9 - SEND_CSD                                                        */
	/* ====================================================================== */

	dbg_test_step = SD_TEST_STEP_CMD9;


	status =
			SDMMC_StartCommand(
					SDMMC1,
					SD_CMD9,
					((uint32_t)dbg_card_rca << 16U),
					SDMMC_RESPONSE_LONG);


	if(status != SDMMC_CMD_OK){

		dbg_test_error = status;
		goto test_end;
	}


	dbg_cmd9_status =
			SDMMC_GetCmdResp2(
					SDMMC1);


	if(dbg_cmd9_status !=
	   SDMMC_CMD_OK){

		dbg_test_error =
				dbg_cmd9_status;

		goto test_end;
	}


	dbg_csd[0] = SDMMC1->RESP1R;
	dbg_csd[1] = SDMMC1->RESP2R;
	dbg_csd[2] = SDMMC1->RESP3R;
	dbg_csd[3] = SDMMC1->RESP4R;


	/* ====================================================================== */
	/* CMD7 - SELECT_CARD                                                     */
	/* ====================================================================== */

	dbg_test_step = SD_TEST_STEP_CMD7;


	status =
			SDMMC_StartCommand(
					SDMMC1,
					SD_CMD7,
					((uint32_t)dbg_card_rca << 16U),
					SDMMC_RESPONSE_SHORT);


	if(status != SDMMC_CMD_OK){

		dbg_test_error = status;
		goto test_end;
	}


	dbg_cmd7_status =
			SDMMC_GetCmdResp1(
					SDMMC1,
					SD_CMD7);


	if(dbg_cmd7_status !=
	   SDMMC_CMD_OK){

		dbg_test_error =
				dbg_cmd7_status;

		goto test_end;
	}


	/*
	 * CMD7 may produce R1b.
	 *
	 * Wait until the card releases DAT0.
	 */
	dbg_cmd7_busy_status =
			SDMMC_WaitBusyD0Release(
					SDMMC1);


	if(dbg_cmd7_busy_status !=
	   SDMMC_CMD_OK){

		dbg_test_error =
				dbg_cmd7_busy_status;

		goto test_end;
	}


	/* ====================================================================== */
	/* CMD16 - SET_BLOCKLEN                                                   */
	/* ====================================================================== */

	/*
	 * SDHC / SDXC always use 512-byte logical blocks.
	 *
	 * CMD16 is required only for SDSC.
	 */
	if(dbg_card_high_capacity == 0U){

		dbg_test_step =
				SD_TEST_STEP_CMD16;


		status =
				SDMMC_StartCommand(
						SDMMC1,
						SD_CMD16,
						512U,
						SDMMC_RESPONSE_SHORT);


		if(status != SDMMC_CMD_OK){

			dbg_test_error = status;
			goto test_end;
		}


		dbg_cmd16_status =
				SDMMC_GetCmdResp1(
						SDMMC1,
						SD_CMD16);


		if(dbg_cmd16_status !=
		   SDMMC_CMD_OK){

			dbg_test_error =
					dbg_cmd16_status;

			goto test_end;
		}
	}


	/* ====================================================================== */
	/* CMD13 - SEND_STATUS                                                    */
	/* ====================================================================== */

	dbg_test_step =
			SD_TEST_STEP_CMD13;


	status =
			SDMMC_StartCommand(
					SDMMC1,
					SD_CMD13,
					((uint32_t)dbg_card_rca << 16U),
					SDMMC_RESPONSE_SHORT);


	if(status != SDMMC_CMD_OK){

		dbg_test_error = status;
		goto test_end;
	}


	dbg_cmd13_status =
			SDMMC_GetCmdResp1(
					SDMMC1,
					SD_CMD13);


	if(dbg_cmd13_status !=
	   SDMMC_CMD_OK){

		dbg_test_error =
				dbg_cmd13_status;

		goto test_end;
	}


	dbg_card_status =
			SDMMC1->RESP1R;


	dbg_card_current_state =
			(uint8_t)(
				(dbg_card_status &
				 SD_R1_CURRENT_STATE_MASK)
				>>
				SD_R1_CURRENT_STATE);


	if((dbg_card_status &
		SD_R1_READY_FOR_DATA) != 0U){

		dbg_card_ready_for_data = 1U;
	}
	else{

		dbg_card_ready_for_data = 0U;
	}


	/*
	 * After CMD7 we want:
	 *
	 * Current State = TRAN
	 * Ready For Data = 1
	 */
	if((dbg_card_current_state !=
		SD_CARD_STATE_TRAN) ||

	   (dbg_card_ready_for_data == 0U)){

		dbg_test_error =
				SDMMC_CMD_ERROR_CARD;

		goto test_end;
	}


	/* ====================================================================== */
	/* Test Passed                                                            */
	/* ====================================================================== */

	dbg_test_step =
			SD_TEST_STEP_DONE;

	dbg_test_pass = 1U;


test_end:

	dbg_sdmmc_cmdr =
			SDMMC1->CMDR;

	dbg_sdmmc_star =
			SDMMC1->STAR;


	while(1){

	}
}
