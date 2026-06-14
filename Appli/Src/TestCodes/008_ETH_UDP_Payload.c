/*
 * 008_ETH_UDP_Payload.c
 *
 *  Created on: 11 Haz 2026
 *      Author: talha
 */

#include <stdint.h>
#include <stdio.h>
#include "stm32h7sxx.h"
#include "stm32h7sxx_gpio_driver.h"
#include "stm32h7sxx_eth_driver.h"

/*
 * Test UDP Port
 *
 * Python Sender should sen UDP broadcast to this port
 */
#define TEST_UDP_PORT 				5005U

/*
 * Debug variables
 *
 * These variables are watched in debugger.
 */
volatile uint8_t  dbg_init_ok = 0U;
volatile uint8_t  dbg_phy_addr = 0U;
volatile uint8_t  dbg_frame_available = 0U;
volatile uint8_t  dbg_udp_available = 0U;

volatile uint32_t dbg_frame_addr = 0U;
volatile uint32_t dbg_frame_length = 0U;
volatile uint32_t dbg_ethertype = 0U;

volatile uint32_t dbg_payload_addr = 0U;
volatile uint32_t dbg_payload_length = 0U;

volatile uint8_t dbg_payload_0 = 0U;
volatile uint8_t dbg_payload_1 = 0U;
volatile uint8_t dbg_payload_2 = 0U;
volatile uint8_t dbg_payload_3 = 0U;
volatile uint8_t dbg_payload_4 = 0U;
volatile uint8_t dbg_payload_5 = 0U;
volatile uint8_t dbg_payload_6 = 0U;
volatile uint8_t dbg_payload_7 = 0U;
volatile uint8_t dbg_payload_8 = 0U;
volatile uint8_t dbg_payload_9 = 0U;
volatile uint8_t dbg_payload_10 = 0U;
volatile uint8_t dbg_payload_11 = 0U;
volatile uint8_t dbg_payload_12 = 0U;
volatile uint8_t dbg_payload_13 = 0U;
volatile uint8_t dbg_payload_14 = 0U;
volatile uint8_t dbg_payload_15 = 0U;

volatile uint32_t dbg_desc_index = 0U;
volatile uint32_t dbg_desc3 = 0U;

volatile uint32_t dbg_maccr = 0U;
volatile uint32_t dbg_macpfr = 0U;
volatile uint32_t dbg_mtlrxqomr = 0U;
volatile uint32_t dbg_dmacrxcr = 0U;

void Test_008_ETH_UDP_Payload(void){
	uint8_t phy_addr = 0U;
	uint8_t phy_found = 0U;
	uint8_t eth_reset_ok = 0U;

	uint8_t mac[ETH_MAC_ADDR_LEN] = {
			0x02U, 0X00U, 0X00U, 0X00U, 0X00U, 0X01U
	};

	uint8_t *frame = 0;
	uint32_t frame_length = 0U;
	uint8_t frame_available = 0U;

	uint8_t *payload = 0;
	uint32_t payload_length = 0U;
	uint8_t udp_available = 0U;

	/*
	 * Config non-cacheable ETH-DMA Memory
	 */
	ETH_MPU_ConfigNonCacheable();

	/*
	 * Basic Ethernet Peripherals Init
	 */
	ETH_PeriClockControl(ENABLE);
	ETH_GPIO_RMII_Init();
	ETH_SelectRMII();

	/*
	 * Ethernet software reset
	 */
	eth_reset_ok = ETH_SoftwareReset();

	if(eth_reset_ok == 0U){
		dbg_init_ok = 0U;
		while(1);
	}

	/*
	 * PHY Address scan
	 */
	phy_found = ETH_PHY_FindAddress(&phy_addr);

	if(phy_found == 0U){
		dbg_init_ok = 0U;
		while(1);
	}

	dbg_phy_addr = phy_addr;

	/*
	 * Wait for link
	 */
	if(ETH_PHY_WaitForLink(phy_addr, ETH_TIMEOUT_VALUE) == 0U){
		dbg_init_ok = 0U;
		while(1);
	}

	/*
	 * Wait for auto-negotiation
	 */
	if(ETH_PHY_WaitAutoNegotiation(phy_addr, ETH_TIMEOUT_VALUE) == 0U){
		dbg_init_ok = 0U;
		while(1);
	}

	/*
	 * Set MAC Adress
	 */
	ETH_SetMACAddress(mac);

	/*
	 * MAC Op mode config
	 *
	 * FES = 1 -> 100 Mbps
	 * DM = 1 -> Full duplex
	 * RE = 1 -> rx enable
	 * TE = 1 -> tx enable
	 */
	ETH_MAC_OPModeConfig(
			ETH_MACCR_FES_MSK |
			ETH_MACCR_DM_MSK  |
			ETH_MACCR_RE_MSK  |
			ETH_MACCR_TE_MSK,

			ETH_MACCR_FES_MSK |
			ETH_MACCR_DM_MSK  |
			ETH_MACCR_RE_MSK  |
			ETH_MACCR_TE_MSK
	);

	/*
	 * MAC packet filterin
	 */
	ETH_MAC_FilterConfig(ENABLE, ETH_MACPFR_PM_MSK);

	/*
	 * Init RX Descriptors before starting DMA <!>
	 */
	ETH_RXDesc_Init();

	/*
	 * MTL RX Queue Configuration
	 *
	 * RSF = 1
	 * Receive store-and-forward mode
	 */
	ETH_MTL_RXQueueConfig(ENABLE, ETH_MTLRXQOMR_RSF_MSK);

	/*
	 * Configure and start RX DMA
	 */
	ETH_DMA_RXConfig();
	ETH_DMA_RXStart();

	dbg_init_ok = 1U;

	while(1){
		frame = 0;
		frame_length = 0U;
		payload = 0;
		payload_length = 0U;

		frame_available = ETH_ReadRawFrame(&frame, &frame_length);

		dbg_frame_available = 1U;

		/*
		 * Debug registers
		 */
		dbg_desc_index = ETH_GetRXCurrentDescIndex();
		dbg_desc3 = ETH_GetRXDesc3(dbg_desc_index);

		dbg_maccr = ETH->MACCR;
		dbg_macpfr = ETH->MACPFR;
		dbg_mtlrxqomr = ETH->MTLRXQOMR;
		dbg_dmacrxcr = ETH->DMACRXCR;

		if(frame_available == 1U){
			dbg_frame_addr = (uint32_t)frame;
			dbg_frame_length = frame_length;

			/*
			 * Read Ethertype
			 */
			dbg_ethertype = ETH_FrameTypeCheck(frame, frame_length);

			/*
			 * Extract UDP
			 */
			udp_available = ETH_GetUDPPayload(frame, frame_length, TEST_UDP_PORT, &payload, &payload_length);

			dbg_udp_available = udp_available;

			if(udp_available == 1U){
				dbg_payload_addr = (uint32_t)payload;
				dbg_payload_length = payload_length;

				/*
				 * Copy first 16 payload bytes to debug variables.
				 *
				 * Example:
				 * Python sends "hello"
				 *
				 * dbg_payload_0 = 'h' = 0x68
				 * dbg_payload_1 = 'e' = 0x65
				 * dbg_payload_2 = 'l' = 0x6C
				 * dbg_payload_3 = 'l' = 0x6C
				 * dbg_payload_4 = 'o' = 0x6F
				 */
				if(payload_length > 0U)  dbg_payload_0  = payload[0];
				if(payload_length > 1U)  dbg_payload_1  = payload[1];
				if(payload_length > 2U)  dbg_payload_2  = payload[2];
				if(payload_length > 3U)  dbg_payload_3  = payload[3];
				if(payload_length > 4U)  dbg_payload_4  = payload[4];
				if(payload_length > 5U)  dbg_payload_5  = payload[5];
				if(payload_length > 6U)  dbg_payload_6  = payload[6];
				if(payload_length > 7U)  dbg_payload_7  = payload[7];
				if(payload_length > 8U)  dbg_payload_8  = payload[8];
				if(payload_length > 9U)  dbg_payload_9  = payload[9];
				if(payload_length > 10U) dbg_payload_10 = payload[10];
				if(payload_length > 11U) dbg_payload_11 = payload[11];
				if(payload_length > 12U) dbg_payload_12 = payload[12];
				if(payload_length > 13U) dbg_payload_13 = payload[13];
				if(payload_length > 14U) dbg_payload_14 = payload[14];
				if(payload_length > 15U) dbg_payload_15 = payload[15];

				/*
				 * Breakpoint here.
				 */
			}

			/*
			 * Give descriptor back to DMA.
			 */
			ETH_ReleaseRXDescriptor();
		}
	}


}
