/*
 * 005_RTOS_ForzaDB_Queue.c
 *
 *  Created on: 6 Eki 2026
 *      Author: talha
 */
#include <stdint.h>
#include <stdio.h>

#include "stm32h7sxx.h"
#include "stm32h7sxx_helpers.h"
#include "stm32h7sxx_gpio_driver.h"
#include "stm32h7sxx_rcc_driver.h"
#include "stm32h7sxx_i2c_driver.h"
#include "stm32h7sxx_eth_driver.h"
#include "stm32h7sxx_eth_net.h"

#include "ssd1306.h"
#include "ForzaDB.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "SEGGER_SYSVIEW.h"


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
 * Private Func Prototypes
 */
static void ForzaMainLoop(void *pvParameters);
static void DisplayTask(void *pvParameters);
static void Delay_ms(uint32_t ms);

/*
 * Private Application Variables
 */
static AppNetworkStatus_t network_status;
static I2C_Handle_t I2C1Handle;
static volatile UBaseType_t dbg_forza_stack_hwm = 0U;

static QueueHandle_t dashboard_queue;


/*
 * Test Application
 */
void Test_005_RTOS_ForzaDB_Queue(void){
	RCC_SystemClock_Config_280MHz_HSI();
	DWT_Init();

	TaskHandle_t forza_task_handle;
	TaskHandle_t display_task_handle;
	BaseType_t status;

	/*
	 * Variables
	 */
	uint8_t phy_addr = 0U;
	uint8_t phy_found = 0U;
	uint8_t eth_reset_ok = 0U;

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

	FPU_ENABLE();

	/*
	 * OLED Init
	 */
	OLED_I2C_GPIO_Init();
	OLED_I2C_Init(&I2C1Handle);

	if(SSD1306_Init(&I2C1Handle) == I2C_OK) network_status.oled_ok = 1U;

	OLED_ShowStartupInfo();
	Delay_ms(OLED_STARTUP_INFO_TIME_MS);

	OLED_ShowNetworkStatus(&network_status, phy_addr);

	/*
	 * MPU Configuration
	 */
	ETH_MPU_ConfigNonCacheable();

	/*
	 * Eth peripherals
	 */
	ETH_PeriClockControl(ENABLE);
	ETH_GPIO_RMII_Init();
	ETH_SelectRMII();

	/*
	 * Eth software reset
	 */
	eth_reset_ok = ETH_SoftwareReset();

	if(eth_reset_ok == 0U){
		SSD1306_Clear();
		SSD1306_DrawString5x7(6U, 6U, "ETH RESET ERR");
		SSD1306_UpdateScreen();

		while(1);
	}

	network_status.eth_reset_ok = 1U;
	OLED_ShowNetworkStatus(&network_status, phy_addr);

	/*
	 * PHY Scan
	 */
	phy_found = ETH_PHY_FindAddress(&phy_addr);

	if(phy_found == 0U){
		SSD1306_Clear();
		SSD1306_DrawString5x7(6U, 6U, "PHY NOT FOUND");
		SSD1306_UpdateScreen();

		while(1);
	}

	network_status.phy_found = 1U;
	OLED_ShowNetworkStatus(&network_status, phy_addr);

	/*
	 * Wait for Ethernet link and auto negation
	 */
	if(ETH_PHY_WaitForLink(phy_addr, ETH_TIMEOUT_VALUE) == 0U){
		SSD1306_Clear();
		SSD1306_DrawString5x7(6U, 6U, "LINK ERR");
		SSD1306_UpdateScreen();

		while(1);
	}

	network_status.link_ok = 1U;
	OLED_ShowNetworkStatus(&network_status, phy_addr);

	if(ETH_PHY_WaitAutoNegotiation(phy_addr, ETH_TIMEOUT_VALUE) == 0U){
		SSD1306_Clear();
		SSD1306_DrawString5x7(6U, 6U, "AUTO NEG ERR");
		SSD1306_UpdateScreen();

		while(1);
	}

	network_status.autoneg_ok = 1U;
	OLED_ShowNetworkStatus(&network_status, phy_addr);

	/*
	 * ETH MAC/DMA Setup
	 */
	ETH_SetMACAddress(myMAC);

	ETH_RXDesc_Init();
	ETH_TXDesc_Init();

	ETH_MTL_RXQueueConfig(ETH_MTLRXQOMR_RSF_MSK, ETH_MTLRXQOMR_RSF_MSK);
	ETH_MTL_TXQueueConfig(ETH_MTLTXQOMR_TSF_MSK | ETH_MTLTXQOMR_TXQEN_ENABLE, ETH_MTLTXQOMR_TSF_MSK | ETH_MTLTXQOMR_TXQEN_MSK);

	ETH_DMA_TXConfig();
	ETH_DMA_RXConfig();

	ETH_DMA_TXStart();
	ETH_DMA_RXStart();

	/*
	 * Promiscuous Mode
	 */
	ETH_MAC_FilterConfig(ETH_MACPFR_PM_MSK, ETH_MACPFR_PM_MSK);
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

	network_status.mac_dma_ok = 1U;
	OLED_ShowNetworkStatus(&network_status, phy_addr);

	//Init End

	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();

	dashboard_queue = xQueueCreate(1, sizeof(ForzaDB_Dashboard_t));

	configASSERT(dashboard_queue != NULL);

	status = xTaskCreate(ForzaMainLoop,
						 "Forza",
						 1024,
						 NULL,
						 3,
						 &forza_task_handle
	);

	configASSERT(status == pdPASS);

	status = xTaskCreate(DisplayTask,
						 "Display",
						 1024,
						 NULL,
						 2,
						 &display_task_handle
	);

	configASSERT(status == pdPASS);

	vTaskStartScheduler();

	while(1);
}

static void ForzaMainLoop(void *pvParameters){
	(void)pvParameters;

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


	uint32_t wait_screen_counter = 0U;

	uint32_t stack_check_counter = 0U;

	while(1){
        frame = 0U;
        frame_length = 0U;
        payload = 0U;
        payload_length = 0U;
        udp_available = 0U;

        frame_available = ETH_ReadRawFrame(&frame, &frame_length);

		if(frame_available == 1U){

			if(ETHNET_ParseEthernetFrame(frame, frame_length, &eth_frame) == 1U){

				if(eth_frame.frame_format == ETHNET_FRAME_ETHERNET_II){

					if(eth_frame.ethertype == ETHNET_ETHERTYPE_ARP){

						if(ETHNET_ParseARPPacket(eth_frame.payload, eth_frame.payload_len, &arp_packet) == 1U){

							if(arp_packet.oper == ETHNET_ARP_OPER_REQUEST){

								if(ETHNET_IsARPRequestForIP(&arp_packet, myIP) == 1U){
									arp_reply_len = ETHNET_BuildARPReply(
											arp_reply_frame,
											myMAC,
											myIP,
											&arp_packet
									);

									if(ETH_SendRawFrame(arp_reply_frame, arp_reply_len) == 1U){
										network_status.arp_ok = 1U;
									} else{
										//dbg_arp_reply_fail_count++;
									}
								}

							} else if(arp_packet.oper == ETHNET_ARP_OPER_REPLY){
								//dbg_arp_reply_count++;
							}
						}

					} else if(eth_frame.ethertype == ETHNET_ETHERTYPE_IPV4){

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
								if(ETH_SendRawFrame(icmp_reply_frame, icmp_reply_len) == 1U){
									network_status.icmp_ok = 1U;
								} else{
									//dbg_icmp_reply_fail_count++;
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

							if(udp_available == 1U){
								network_status.udp_ok = 1U;

								if(payload_length == FORZADB_PACKET_SIZE){
									if(ForzaDB_ParsePacket(payload, payload_length, &forza_packet) == 1U){
										ForzaDB_BuildDashboardData(&forza_packet, &forza_dash);
										network_status.forza_ok = 1U;

										xQueueOverwrite(dashboard_queue, &forza_dash);

									} else{
										//dbg_forzadb_parse_fail_count++;
									}
								} else{
									wait_screen_counter++;

									if(wait_screen_counter >= OLED_WAIT_UPDATE_PERIOD_COUNT){
										wait_screen_counter = 0U;
										//OLED_ShowWaitingForza(dbg_udp_match_count, dbg_forzadb_packet_count);
									}
								}
							}
						}
					}
				} else if (eth_frame.frame_format == ETHNET_FRAME_IEEE_802_3){
					//dbg_ethnet_8023_count++;
				} else{
					//dbg_ethnet_unknown_count++;
				}
			} else{
				//dbg_ethnet_parse_fail++;
			}

			ETH_ReleaseRXDescriptor();
		}

		stack_check_counter++;

		if(stack_check_counter >= 1000U){
			stack_check_counter = 0U;
			dbg_forza_stack_hwm = uxTaskGetStackHighWaterMark(NULL);
		}


		vTaskDelay(pdMS_TO_TICKS(1));
	}
}

static void DisplayTask(void *pvParameters){
	(void)pvParameters;

	ForzaDB_Dashboard_t display_data;
	TickType_t last_wake_time;

	last_wake_time = xTaskGetTickCount();

	while(1){
		if(xQueuePeek(dashboard_queue, &display_data, 0) == pdPASS){
			OLED_ShowForzaDashboard(&display_data);
		}

		vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(OLED_DASH_UPDATE_PERIOD_MS));
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
