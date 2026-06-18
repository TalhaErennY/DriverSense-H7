/*
 * 020_ForzaDB_App.c
 *
 *  Created on: 18 Haz 2026
 *      Author: talha
 */

#include <stdint.h>
#include <stdio.h>

#include "stm32h7sxx.h"
#include "stm32h7sxx_gpio_driver.h"
#include "stm32h7sxx_i2c_driver.h"
#include "stm32h7sxx_eth_driver.h"
#include "stm32h7sxx_eth_net.h"

#include "ssd1306.h"
#include "ForzaDB.h"

/*
 * UDP Port
 *
 * Forza Data Out Port should be set to this port.
 */
#define TEST_UDP_PORT						FORZADB_UDP_PORT

/*
 * Simple delay calibration.
 */
#define DELAY_MS_LOOP_COUNT					12000U

/*
 * Local IP/MAC selection
 *
 * Forza Data Out IP should be set to this IP address.
 */
uint8_t myIP[ETHNET_IPV4_ADDR_LEN] = {
		192U, 168U, 1U, 50U
};

uint8_t myMAC[ETHNET_MAC_ADDR_LEN] = {
		0x02U, 0x00U, 0x00U, 0x00U, 0x00U, 0x01U
};

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

volatile uint32_t dbg_arp_request_count = 0U;
volatile uint32_t dbg_arp_reply_count = 0U;
volatile uint32_t dbg_arp_for_me_count = 0U;
volatile uint32_t dbg_arp_reply_sent_count = 0U;
volatile uint32_t dbg_arp_reply_fail_count = 0U;

volatile uint32_t dbg_icmp_request_count = 0U;
volatile uint32_t dbg_icmp_reply_sent_count = 0U;
volatile uint32_t dbg_icmp_reply_fail_count = 0U;
volatile uint32_t dbg_last_icmp_reply_len = 0U;
volatile uint32_t dbg_icmp_tx_stage = 0U;
volatile uint32_t dbg_icmp_send_ret = 0U;

volatile uint32_t dbg_udp_match_count = 0U;

volatile uint32_t dbg_forzadb_packet_count = 0U;
volatile uint32_t dbg_forzadb_bad_size_count = 0U;
volatile uint32_t dbg_forzadb_parse_fail_count = 0U;

volatile uint16_t dbg_forzadb_speed_kmh = 0U;
volatile uint16_t dbg_forzadb_rpm = 0U;
volatile uint16_t dbg_forzadb_max_rpm = 0U;
volatile uint8_t  dbg_forzadb_gear = 0U;
volatile uint8_t  dbg_forzadb_accel = 0U;
volatile uint8_t  dbg_forzadb_brake = 0U;
volatile int8_t   dbg_forzadb_steer = 0;
volatile uint8_t  dbg_forzadb_fuel = 0U;
volatile uint8_t  dbg_forzadb_slip = 0U;
volatile uint8_t  dbg_forzadb_warning = 0U;

volatile int32_t  dbg_forzadb_is_race_on = 0;
volatile uint32_t dbg_forzadb_timestamp_ms = 0U;

volatile uint32_t dbg_error_code = 0U;
volatile uint32_t dbg_init_step = 0U;
volatile uint8_t  dbg_oled_status = 0U;

/*
 * Private helper prototypes
 */
static void Delay_ms(uint32_t ms);
static void APP_UpdateForzaDebug(const ForzaDB_Packet_t *pPacket, const ForzaDB_Dashboard_t *pDash);

/******************************************************************************************************
 * 								Test Application
 ******************************************************************************************************/
void Test_020_ForzaDB_App_Test(void){
	I2C_Handle_t I2C1Handle;

	uint8_t phy_addr = 0U;
	uint8_t phy_found = 0U;
	uint8_t eth_reset_ok = 0U;

	uint8_t *frame = 0U;
	uint32_t frame_length = 0U;
	uint8_t frame_available = 0U;

	const uint8_t *payload = 0U;
	uint32_t payload_length = 0U;
	uint8_t udp_available = 0U;

	ETHNET_Frame_t eth_frame;
	ETHNET_ARP_Packet_t arp_packet;

	uint8_t arp_reply_frame[ETH_TX_BUF_SIZE];
	uint32_t arp_reply_len = 0U;

	uint8_t icmp_reply_frame[ETH_TX_BUF_SIZE];
	uint32_t icmp_reply_len = 0U;

	ForzaDB_Packet_t forza_packet;
	ForzaDB_Dashboard_t forza_dash;

	AppNetworkStatus_t network_status;

	uint32_t last_oled_timestamp = 0U;
	uint32_t wait_screen_counter = 0U;

	FPU_ENABLE();

	network_status.oled_ok = 0U;
	network_status.eth_reset_ok = 0U;
	network_status.phy_found = 0U;
	network_status.link_ok = 0U;
	network_status.autoneg_ok = 0U;
	network_status.mac_dma_ok = 0U;
	network_status.arp_ok = 0U;
	network_status.icmp_ok = 0U;
	network_status.udp_ok = 0U;
	network_status.forza_ok = 0U;

	/*
	 * OLED Init
	 */
	OLED_I2C_GPIO_Init();
	OLED_I2C_Init(&I2C1Handle);

	dbg_oled_status = SSD1306_Init(&I2C1Handle);

	if(dbg_oled_status == I2C_OK) network_status.oled_ok = 1U;

	OLED_ShowStartupInfo();
	Delay_ms(OLED_STARTUP_INFO_TIME_MS);

	OLED_ShowNetworkStatus(&network_status, phy_addr);

	/*
	 * MPU configuration.
	 */
	dbg_init_step = 1U;
	ETH_MPU_ConfigNonCacheable();

	/*
	 * Ethernet peripheral init.
	 */
	dbg_init_step = 2U;
	ETH_PeriClockControl(ENABLE);
	ETH_GPIO_RMII_Init();
	ETH_SelectRMII();

	/*
	 * Ethernet software reset
	 */
	dbg_init_step = 3U;
	eth_reset_ok = ETH_SoftwareReset();

	if(eth_reset_ok == 0U){
		dbg_error_code = 1U;
		dbg_init_ok = 0U;

		SSD1306_Clear();
		SSD1306_DrawString5x7(6U, 6U, "ETH RESET ERR");
		SSD1306_UpdateScreen();

		while(1);
	}

	network_status.eth_reset_ok = 1U;
	OLED_ShowNetworkStatus(&network_status, phy_addr);

	/*
	 * 5) PHY address scan.
	 */
	dbg_init_step = 4U;
	phy_found = ETH_PHY_FindAddress(&phy_addr);

	if(phy_found == 0U){
		dbg_error_code = 2U;
		dbg_init_ok = 0U;

		SSD1306_Clear();
		SSD1306_DrawString5x7(6U, 6U, "PHY NOT FOUND");
		SSD1306_UpdateScreen();

		while(1);
	}

	dbg_phy_addr = phy_addr;
	network_status.phy_found = 1U;
	OLED_ShowNetworkStatus(&network_status, phy_addr);

	/*
	 * Wait for Ethernet link and auto negation
	 */
	dbg_init_step = 5U;

	if(ETH_PHY_WaitForLink(phy_addr, ETH_TIMEOUT_VALUE) == 0U){
		dbg_error_code = 3U;
		dbg_init_ok = 0U;

		SSD1306_Clear();
		SSD1306_DrawString5x7(6U, 6U, "LINK ERR");
		SSD1306_UpdateScreen();

		while(1);
	}

	network_status.link_ok = 1U;
	OLED_ShowNetworkStatus(&network_status, phy_addr);

	dbg_init_step = 6U;

	if(ETH_PHY_WaitAutoNegotiation(phy_addr, ETH_TIMEOUT_VALUE) == 0U){
		dbg_error_code = 4U;
		dbg_init_ok = 0U;

		SSD1306_Clear();
		SSD1306_DrawString5x7(6U, 6U, "AUTO NEG ERR");
		SSD1306_UpdateScreen();

		while(1);
	}

	network_status.autoneg_ok = 1U;
	OLED_ShowNetworkStatus(&network_status, phy_addr);

	/*
	 * Ethernet MAC / DMA setup.
	 *
	 * Current system uses both RX and TX descriptors.
	 * TX is required for ARP reply and ICMP echo reply.
	 */
	dbg_init_step = 7U;
	ETH_SetMACAddress(myMAC);

	dbg_init_step = 8U;
	ETH_RXDesc_Init();
	ETH_TXDesc_Init();

	dbg_init_step = 9U;
	ETH_MTL_RXQueueConfig(ETH_MTLRXQOMR_RSF_MSK, ETH_MTLRXQOMR_RSF_MSK);
	ETH_MTL_TXQueueConfig(ETH_MTLTXQOMR_TSF_MSK | ETH_MTLTXQOMR_TXQEN_ENABLE, ETH_MTLTXQOMR_TSF_MSK | ETH_MTLTXQOMR_TXQEN_MSK);

	dbg_init_step = 10U;
	ETH_DMA_TXConfig();
	ETH_DMA_RXConfig();

	dbg_init_step = 11U;
	ETH_DMA_RXStart();
	ETH_DMA_TXStart();

	/*
	 * Promiscuous mode
	 */
	dbg_init_step = 12U;
	ETH_MAC_FilterConfig(ETH_MACPFR_PM_MSK, ETH_MACPFR_PM_MSK);

	dbg_init_step = 13U;
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

	dbg_init_step = 14U;
	dbg_error_code = 0U;
	dbg_init_ok = 1U;

	network_status.mac_dma_ok = 1U;
	OLED_ShowNetworkStatus(&network_status, phy_addr);

	while(1){
		frame = 0U;
		frame_length = 0U;
		payload = 0U;
		payload_length = 0U;
		udp_available = 0U;

		frame_available = ETH_ReadRawFrame(&frame, &frame_length);

		dbg_frame_available = frame_available;

		if(frame_available == 1U){
			dbg_frame_length = frame_length;

			if(ETHNET_ParseEthernetFrame(frame, frame_length, &eth_frame) == 1U){
				dbg_ethnet_parse_ok++;

				if(eth_frame.frame_format == ETHNET_FRAME_ETHERNET_II){
					if(eth_frame.ethertype == ETHNET_ETHERTYPE_ARP){
						dbg_ethnet_arp_count++;

						if(ETHNET_ParseARPPacket(eth_frame.payload, eth_frame.payload_len, &arp_packet) == 1U){
							if(arp_packet.oper == ETHNET_ARP_OPER_REQUEST){
								dbg_arp_request_count++;

								if(ETHNET_IsARPRequestForIP(&arp_packet, myIP) == 1U){
									dbg_arp_for_me_count++;

									arp_reply_len = ETHNET_BuildARPReply(
											arp_reply_frame,
											myMAC,
											myIP,
											&arp_packet
									);

									if(ETH_SendRawFrame(arp_reply_frame, arp_reply_len) == 1U){
										dbg_arp_reply_sent_count++;
										network_status.arp_ok = 1U;
									} else{
										dbg_arp_reply_fail_count++;
									}
								}
							} else if(arp_packet.oper == ETHNET_ARP_OPER_REPLY){
								dbg_arp_reply_count++;
							}
						}
					} else if(eth_frame.ethertype == ETHNET_ETHERTYPE_IPV4){
						dbg_ethnet_ipv4_count++;

						if(ETHNET_IsIPv4PacketForIP(frame, frame_length, myIP) == 1U){
							/*
							 * ICMP Echo Reply
							 */
							icmp_reply_len = ETHNET_BuildICMPEchoReply(
									icmp_reply_frame,
									ETH_TX_BUF_SIZE,
									frame,
									frame_length,
									myMAC,
									myIP
							);

							if(icmp_reply_len > 0U){
								dbg_icmp_request_count++;

								dbg_last_icmp_reply_len = icmp_reply_len;

								dbg_icmp_tx_stage = 1U;

								dbg_icmp_send_ret = ETH_SendRawFrame(icmp_reply_frame, icmp_reply_len);

								dbg_icmp_tx_stage = 2U;

								if(ETH_SendRawFrame(icmp_reply_frame, icmp_reply_len) == 1U){
									dbg_icmp_reply_sent_count++;
									network_status.icmp_ok = 1U;
								} else{
									dbg_icmp_reply_fail_count++;
								}
							}

							/*
							 * UDP / Forza Data Out with IPv4/UDP lenght chekcs and chekcsums
							 */
							udp_available = ETHNET_GetUDPPayload(
									frame,
									frame_length,
									TEST_UDP_PORT,
									&payload,
									&payload_length
							);

							dbg_udp_available = udp_available;

							if(udp_available == 1U){
								dbg_payload_length = payload_length;
								dbg_udp_match_count++;

								network_status.udp_ok = 1U;

								if(payload_length == FORZADB_PACKET_SIZE){
									if(ForzaDB_ParsePacket(payload, payload_length, &forza_packet) == 1U){
										dbg_forzadb_packet_count++;

										ForzaDB_BuildDashboardData(&forza_packet, &forza_dash);
										APP_UpdateForzaDebug(&forza_packet, &forza_dash);

										network_status.forza_ok = 1U;

										/*
										 * OLED update is limited by Forza timestamp.
										 */
										if((forza_packet.TimestampMS - last_oled_timestamp) >= OLED_DASH_UPDATE_PERIOD_MS){
											last_oled_timestamp = forza_packet.TimestampMS;
											OLED_ShowForzaDashboard(&forza_dash);
										}
									} else{
										dbg_forzadb_parse_fail_count++;
									}
								} else{
									dbg_forzadb_bad_size_count++;

									wait_screen_counter++;

									if(wait_screen_counter >= OLED_WAIT_UPDATE_PERIOD_COUNT){
										wait_screen_counter = 0U;
										OLED_ShowWaitingForza(dbg_udp_match_count, dbg_forzadb_packet_count);
									}
								}
							}
						}
					}
				} else if (eth_frame.frame_format == ETHNET_FRAME_IEEE_802_3){
					dbg_ethnet_8023_count++;
				} else{
					dbg_ethnet_unknown_count++;
				}
			} else{
				dbg_ethnet_parse_fail++;
			}

			ETH_ReleaseRXDescriptor();
		}
	}
}

/******************************************************************************************************
 * 								Private Helper Functions
 ******************************************************************************************************/
static void Delay_ms(uint32_t ms){
	volatile uint32_t i;

	while(ms > 0U){
		for(i = 0U; i < DELAY_MS_LOOP_COUNT; i++){
			__asm volatile ("nop");
		}

		ms--;
	}
}

static void APP_UpdateForzaDebug(const ForzaDB_Packet_t *pPacket, const ForzaDB_Dashboard_t *pDash){
	if((pPacket == 0U) || (pDash == 0U)) return;

	dbg_forzadb_is_race_on = pPacket->IsRaceOn;
	dbg_forzadb_timestamp_ms = pPacket->TimestampMS;

	dbg_forzadb_speed_kmh = pDash->SpeedKmh;
	dbg_forzadb_rpm = pDash->Rpm;
	dbg_forzadb_max_rpm = pDash->MaxRpm;
	dbg_forzadb_gear = pDash->Gear;
	dbg_forzadb_accel = pDash->AccelPercent;
	dbg_forzadb_brake = pDash->BrakePercent;
	dbg_forzadb_steer = pDash->Steer;
	dbg_forzadb_fuel = pDash->FuelPercent;
	dbg_forzadb_slip = pDash->SlipPercent;
	dbg_forzadb_warning = pDash->WarningLevel;
}
