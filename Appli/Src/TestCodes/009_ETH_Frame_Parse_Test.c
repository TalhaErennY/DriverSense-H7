/*
 * 009_ETH_Frame_Parse_Test.c
 *
 *  Created on: 13 Haz 2026
 *      Author: talha
 */

#include <stdio.h>
#include <stdint.h>

#include "stm32h7sxx.h"
#include "stm32h7sxx_eth_driver.h"
#include "stm32h7sxx_eth_net.h"

/*
 * Test UDP Port
 */
#define TEST_UDP_PORT				5005U

/*
 * Debug variables
 */
volatile uint8_t  dbg_init_ok = 0U;
volatile uint8_t  dbg_phy_addr = 0U;
volatile uint8_t  dbg_frame_available = 0U;
volatile uint8_t  dbg_udp_available = 0U;

volatile uint32_t dbg_frame_length = 0U;
volatile uint32_t dbg_payload_length = 0U;

volatile uint32_t dbg_ethnet_parse_ok = 0U;
volatile uint32_t dbg_ethnet_parse_fail = 0U;

volatile uint32_t dbg_ethnet_ipv4_count = 0U;
volatile uint32_t dbg_ethnet_arp_count = 0U;
volatile uint32_t dbg_ethnet_8023_count = 0U;
volatile uint32_t dbg_ethnet_unknown_count = 0U;

volatile uint32_t dbg_udp_match_count = 0U;

volatile uint16_t dbg_last_type_or_len = 0U;
volatile uint16_t dbg_last_ethertype = 0U;

volatile uint32_t dbg_error_code = 0U;
volatile uint32_t dbg_init_step = 0U;

void Test_009_ETH_Frame_Parse(void){
	uint8_t phy_addr = 0U;
	uint8_t phy_found = 0U;
	uint8_t eth_reset_ok = 0U;

	uint8_t mac[ETHNET_MAC_ADDR_LEN] = {
			0x02U, 0x00U, 0x00U, 0x00U, 0x00U, 0x01U
	};

	uint8_t *frame = 0;
	uint32_t frame_length = 0U;
	uint8_t frame_available = 0U;

	const uint8_t *payload = 0;
	uint32_t payload_length = 0U;
	uint8_t udp_available = 0U;

	ETHNET_Frame_t eth_frame;

	//MPU Config for DMA
	dbg_init_step = 1U;
	ETH_MPU_ConfigNonCacheable();

	//Perioherals Init
	dbg_init_step = 2U;
	ETH_PeriClockControl(ENABLE);
	ETH_GPIO_RMII_Init();
	ETH_SelectRMII();

	/*
	 * ETH Software Reset
	 */
	dbg_init_step = 3U;
	eth_reset_ok = ETH_SoftwareReset();

	if(eth_reset_ok == 0U){
		dbg_error_code = 1U;
		dbg_init_ok = 0U;

		while(1);
	}

	/*
	 * PHY Address scan
	 */
	dbg_init_step = 4U;
	phy_found = ETH_PHY_FindAddress(&phy_addr);

	if(phy_found == 0U){
		dbg_error_code = 2U;
		dbg_init_ok = 0U;

		while(1);
	}

	dbg_phy_addr = phy_addr;

	/*
	 * Wait for Ethernet link
	 */
	dbg_init_step = 5U;

	if(ETH_PHY_WaitForLink(phy_addr, ETH_TIMEOUT_VALUE) == 0U){
		dbg_error_code = 3U;
		dbg_init_ok = 0U;

		while(1);
	}

	/*
	 * Wait for auto-negation
	 */
	dbg_init_step = 6U;

	if(ETH_PHY_WaitAutoNegotiation(phy_addr, ETH_TIMEOUT_VALUE) == 0U){
		dbg_error_code = 4U;
		dbg_init_ok = 0U;

		while(1);
	}

	/*
	 * Ethernet MAC/DMA setup
	 */
	dbg_init_step = 7U;
	ETH_SetMACAddress(mac);

	dbg_init_step = 8U;
	ETH_MAC_OPModeConfig(
			ETH_MACCR_FES_MSK 	|
			ETH_MACCR_DM_MSK	|
			ETH_MACCR_RE_MSK	|
			ETH_MACCR_TE_MSK,

			ETH_MACCR_FES_MSK	|
			ETH_MACCR_DM_MSK	|
			ETH_MACCR_RE_MSK	|
			ETH_MACCR_TE_MSK
	);

	/*
	 * PM Mode
	 */
	dbg_init_step = 9U;
	ETH_MAC_FilterConfig(ENABLE, ETH_MACPFR_PM_MSK);

	dbg_init_step = 10U;
	ETH_RXDesc_Init();

	dbg_init_step = 11U;
	ETH_MTL_RXQueueConfig(ENABLE, ETH_MTLRXQOMR_RSF_MSK);

	dbg_init_step = 12U;
	ETH_DMA_RXConfig();
	ETH_DMA_RXStart();

	dbg_init_step = 13U;
	dbg_error_code = 0U;
	dbg_init_ok = 1U;

	while(1){
		frame = 0;
		frame_length = 0U;

		payload = 0;
		payload_length = 0U;
		udp_available = 0U;

		/*
		 * Read RAW Eth frame from RX Descriptor
		 */
		frame_available = ETH_ReadRawFrame(&frame, &frame_length);

		dbg_frame_available = frame_available;

		if(frame_available == 1U){
			dbg_frame_length = frame_length;

			/*
			 * Parse Frame
			 */
			if(ETHNET_ParseEthernetFrame(frame, frame_length, &eth_frame) == 1U){
				dbg_ethnet_parse_ok++;

				dbg_last_type_or_len = eth_frame.type_or_len;
				dbg_last_ethertype = eth_frame.ethertype;

				if(eth_frame.frame_format == ETHNET_FRAME_ETHERNET_II){
					if(eth_frame.ethertype == ETHNET_ETHERTYPE_IPV4){
						dbg_ethnet_ipv4_count++;
					} else if(eth_frame.ethertype == ETHNET_ETHERTYPE_ARP){
						dbg_ethnet_arp_count++;
					} else{
						dbg_ethnet_unknown_count++;
					}
				} else if(eth_frame.frame_format == ETHNET_FRAME_IEEE_802_3){
					dbg_ethnet_8023_count++;
				} else{
					dbg_ethnet_unknown_count++;
				}
			} else{
				dbg_ethnet_parse_fail++;
			}

			/*
			 * Type UDP payload extraction
			 */
			udp_available = ETHNET_GetUDPPayload(frame, frame_length, TEST_UDP_PORT, &payload, &payload_length);

			dbg_udp_available = udp_available;

			if(udp_available == 1U){
				dbg_payload_length = payload_length;
				dbg_udp_match_count++;
			}

			ETH_ReleaseRXDescriptor();

		}
	}



}

