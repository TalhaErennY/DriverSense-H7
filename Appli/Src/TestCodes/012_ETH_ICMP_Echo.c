/*
 * 012_ETH_ICMP_Echo.c
 *
 *  Created on: 17 Haz 2026
 *      Author: talha
 */

#include <stdio.h>
#include <stdint.h>

#include "stm32h7sxx.h"
#include "stm32h7sxx_eth_driver.h"
#include "stm32h7sxx_eth_net.h"

//Test UDP Port
#define TEST_UDP_PORT 				5005U

//Custom IP/MAC
uint8_t myIP[ETHNET_IPV4_ADDR_LEN] = {192U, 168U, 1U, 50U};
uint8_t myMAC[ETHNET_MAC_ADDR_LEN] = {0x02U, 0x00U, 0x00U, 0x00U, 0x00U, 0x01U};

//Debugger Variables
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

volatile uint32_t dbg_arp_parse_fail = 0U;
volatile uint32_t dbg_arp_request_count = 0U;
volatile uint32_t dbg_arp_reply_count = 0U;
volatile uint32_t dbg_arp_for_me_count = 0U;
volatile uint32_t dbg_arp_reply_sent_count = 0U;
volatile uint32_t dbg_arp_reply_fail_count = 0U;

volatile uint32_t dbg_icmp_request_count = 0U;
volatile uint32_t dbg_icmp_reply_sent_count = 0U;
volatile uint32_t dbg_icmp_reply_fail_count = 0U;

volatile uint32_t dbg_udp_match_count = 0U;

volatile uint32_t dbg_error_code = 0U;
volatile uint32_t dbg_init_step = 0U;

void Test_012_ETH_ICMP_Echo(void){
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

	uint8_t arp_reply_frame[64];
	uint32_t arp_reply_len = 0U;

	uint8_t icmp_reply_frame[ETH_TX_BUF_SIZE];
	uint32_t icmp_reply_len = 0U;

	/*
	 * MPU Config
	 */
	dbg_init_step = 1U;
	ETH_MPU_ConfigNonCacheable();

	/*
	 * Eth peri clock, GPIO RMII Init and SBS RMII select
	 */
	dbg_init_step = 2U;
	ETH_PeriClockControl(ENABLE);
	ETH_GPIO_RMII_Init();
	ETH_SelectRMII();

	/*
	 * Software Reset
	 */
	dbg_init_step = 3U;
	eth_reset_ok = ETH_SoftwareReset();

	if(eth_reset_ok == 0U){
		dbg_error_code = 1U;
		dbg_init_ok = 0U;
		while(1);
	}

	/*
	 * PHY Address Scan
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
	 * Wait for link and auto negotiation
	 */
	dbg_init_step = 5U;

	if(ETH_PHY_WaitForLink(phy_addr, ETH_TIMEOUT_VALUE) == 0U){
		dbg_error_code = 3U;
		dbg_init_ok = 0U;
		while(1);
	}

	if(ETH_PHY_WaitAutoNegotiation(phy_addr, ETH_TIMEOUT_VALUE) == 0U){
		dbg_error_code = 4U;
		dbg_init_ok = 0U;
		while(1);
	}

	/*
	 * MAC Set
	 */
	dbg_init_step = 6U;
	ETH_SetMACAddress(myMAC);

	/*
	 * Descriptor Init
	 */
	dbg_init_step = 7U;
	ETH_RXDesc_Init();
	ETH_TXDesc_Init();

	/*
	 * MTL RX/TX Queue Config
	 */
	dbg_init_step = 8U;
	ETH_MTL_RXQueueConfig(ETH_MTLRXQOMR_RSF_MSK, ETH_MTLRXQOMR_RSF_MSK);
	ETH_MTL_TXQueueConfig(ETH_MTLTXQOMR_TSF_MSK | ETH_MTLTXQOMR_TXQEN_ENABLE, ETH_MTLTXQOMR_TSF_MSK | ETH_MTLTXQOMR_TXQEN_MSK);

	/*
	 * DMA TX/RX Config
	 */
	dbg_init_step = 9U;
	ETH_DMA_TXConfig();
	ETH_DMA_RXConfig();

	/*
	 * Start DMA TX/RX
	 */
	dbg_init_step = 10U;
	ETH_DMA_RXStart();
	ETH_DMA_TXStart();

	/*
	 * MAC Filter Configuration
	 */
	dbg_init_step = 11U;
	ETH_MAC_FilterConfig(ETH_MACPFR_PM_MSK, ETH_MACPFR_PM_MSK);

	/*
	 * Enable MAC TX/RX
	 */
	dbg_init_step = 12U;
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

	dbg_error_code = 0U;
	dbg_init_ok = 1U;

	while(1){
		frame = 0U;
		frame_length = 0U;
		frame_available = 0U;

		payload = 0U;
		payload_length = 0U;
		udp_available = 0U;

		arp_reply_len = 0U;
		icmp_reply_len = 0U;

		frame_available = ETH_ReadRawFrame(&frame, &frame_length);
		dbg_frame_available = frame_available;

		if(frame_available == 1U){
			dbg_frame_length = frame_length;

			/*
			 * Ethernet Frame Parse
			 */
			if(ETHNET_ParseEthernetFrame(frame, frame_length, &eth_frame) == 1U){
				dbg_ethnet_parse_ok++;

				if(eth_frame.frame_format == ETHNET_FRAME_ETHERNET_II){
					/*
					 * ARP Packet Handling
					 */
					if(eth_frame.ethertype == ETHNET_ETHERTYPE_ARP){
						if(ETHNET_ParseARPPacket(eth_frame.payload, eth_frame.payload_len, &arp_packet) == 1U){
							dbg_ethnet_arp_count++;

							if(arp_packet.oper == ETHNET_ARP_OPER_REQUEST){
								dbg_arp_request_count++;
							} else if(arp_packet.oper == ETHNET_ARP_OPER_REPLY){
								dbg_arp_reply_count++;
							}

							/*
							 * Check ARP request for my IP
							 */
							if(ETHNET_IsARPRequestForIP(&arp_packet, myIP) == 1U){
								dbg_arp_for_me_count++;

								/*
								 * Build ARP Reply
								 */
								arp_reply_len = ETHNET_BuildARPReply(
										arp_reply_frame,
										myMAC,
										myIP,
										&arp_packet
								);

								/*
								 * Send ARP Reply
								 */
								if(arp_reply_len != 0U){
									if(ETH_SendRawFrame(arp_reply_frame, arp_reply_len) == 1U){
										dbg_arp_reply_sent_count++;
									} else{
										dbg_arp_reply_fail_count++;
									}
								}
							}
						} else{
							dbg_arp_parse_fail++;
						}
					}

					/*
					 * IPv4 Packet Handling
					 */
					else if(eth_frame.ethertype == ETHNET_ETHERTYPE_IPV4){
						dbg_ethnet_ipv4_count++;

						/*
						 * Only process IPv4 packets addressed to my IP
						 */
						if(ETHNET_IsIPv4PacketForIP(frame, frame_length, myIP) == 1U){
							/*
							 * ICMP Echo Reply Handling
							 *
							 * If this IPv4 packet is not ICMP echo request,
							 * ETHNET_BuildICMPEchoReply returns 0.
							 */
							icmp_reply_len = ETHNET_BuildICMPEchoReply(
									icmp_reply_frame,
									ETH_TX_BUF_SIZE,
									frame,
									frame_length,
									myMAC,
									myIP
							);

							if(icmp_reply_len != 0U){
								dbg_icmp_request_count++;

								if(ETH_SendRawFrame(icmp_reply_frame, icmp_reply_len) == 1U){
									dbg_icmp_reply_sent_count++;
								} else{
									dbg_icmp_reply_fail_count++;
								}
							}

							/*
							 * UDP Payload Handling
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

								/*
								 * Telemetry parser will be added here.
								 */
							}
						}
					}

					else{
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
			 * Release RX descriptor after all processing is finished.
			 */
			ETH_ReleaseRXDescriptor();
		}
	}
}

