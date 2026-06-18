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
 * OLED update selection
 */
#define OLED_STARTUP_INFO_TIME_MS			3000U
#define OLED_DASH_UPDATE_PERIOD_MS			100U
#define OLED_WAIT_UPDATE_PERIOD_COUNT		100U

/*
 * Simple delay calibration.
 *
 * Bu değer tam gerçek ms olmayabilir.
 * Şu an amaç açılış ekranını kısa süre göstermek.
 */
#define DELAY_MS_LOOP_COUNT					12000U

/*
 * Local IP/MAC selection
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
 * FPU register
 */
#define SCB_CPACR_ADDR						(0xE000ED88UL)
#define SCB_CPACR							(*(volatile uint32_t *)SCB_CPACR_ADDR)

/*
 * Network OLED status
 */
typedef struct{
	uint8_t oled_ok;
	uint8_t eth_reset_ok;
	uint8_t phy_found;
	uint8_t link_ok;
	uint8_t autoneg_ok;
	uint8_t mac_dma_ok;
	uint8_t arp_ok;
	uint8_t icmp_ok;
	uint8_t udp_ok;
	uint8_t forza_ok;
} AppNetworkStatus_t;

/*
 * Private helper prototypes
 */
static void FPU_Enable(void);
static void Delay_ms(uint32_t ms);

static void OLED_I2C_GPIO_Init(void);
static void OLED_I2C_Init(I2C_Handle_t *pI2CHandle);

static void OLED_ShowStartupInfo(void);
static void OLED_ShowNetworkStatus(const AppNetworkStatus_t *pStatus, uint8_t phy_addr);
static void OLED_ShowWaitingForza(uint32_t udp_count, uint32_t forza_count);
static void OLED_ShowForzaDashboard(const ForzaDB_Dashboard_t *pDash);

static void APP_UpdateForzaDebug(const ForzaDB_Packet_t *pPacket, const ForzaDB_Dashboard_t *pDash);

/******************************************************************************************************
 * 								Private Helper Functions
 ******************************************************************************************************/
static void FPU_Enable(void){
	/*
	 * Enable CP10 and CP11 full access.
	 * Required before using floating point instructions on Cortex-M7.
	 */
	SCB_CPACR |= (0xFU << 20U);

	/*
	 * Data and instruction synchronization barriers.
	 */
	__asm volatile ("dsb");
	__asm volatile ("isb");
}

static void Delay_ms(uint32_t ms){
	volatile uint32_t i;

	while(ms > 0U){
		for(i = 0U; i < DELAY_MS_LOOP_COUNT; i++){
			__asm volatile ("nop");
		}

		ms--;
	}
}

/*
 * OLED I2C GPIO init
 *
 * Assumption:
 * I2C1 SCL -> PB8
 * I2C1 SDA -> PB9
 * AF4
 *
 * Eğer sen OLED'i farklı pine bağladıysan sadece bu fonksiyonu değiştir.
 */
static void OLED_I2C_GPIO_Init(void){
	GPIO_Handle_t GPIOI2C;

	GPIO_PeriClockControl(GPIOB, ENABLE);

	GPIOI2C.pGPIOx = GPIOB;

	GPIOI2C.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_AF;
	GPIOI2C.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	GPIOI2C.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_VHIGH;
	GPIOI2C.GPIO_PinConfig.GPIO_PinPuPdCtrl = GPIO_PUPD_PU;
	GPIOI2C.GPIO_PinConfig.GPIO_PinAltFuncMode = 4U;

	/*
	 * PB8 -> I2C1_SCL
	 */
	GPIOI2C.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8;
	GPIO_Init(&GPIOI2C);

	/*
	 * PB9 -> I2C1_SDA
	 */
	GPIOI2C.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_9;
	GPIO_Init(&GPIOI2C);
}

static void OLED_I2C_Init(I2C_Handle_t *pI2CHandle){
	pI2CHandle->pI2Cx = I2C1;

	pI2CHandle->I2C_Config.I2C_Timing = I2C_TIMING_400KHZ;
	pI2CHandle->I2C_Config.I2C_DeviceAddress = 0U;
	pI2CHandle->I2C_Config.I2C_AddressingMode = I2C_ADDRMODE_7BIT;
	pI2CHandle->I2C_Config.I2C_NoStretchMode = I2C_STRETCH_ENABLE;

	I2C_Init(pI2CHandle);
}

static void OLED_ShowStartupInfo(void){
	SSD1306_Clear();

	SSD1306_DrawRect(0U, 0U, 128U, 64U, SSD1306_PIXEL_ON);

	SSD1306_DrawString5x7(6U, 6U,  "DRIVESENSE-H7");
	SSD1306_DrawString5x7(6U, 18U, "FORZA UDP DASH");
	SSD1306_DrawString5x7(6U, 30U, "IP 192.168.1.50");
	SSD1306_DrawString5x7(6U, 42U, "PORT 5005");
	SSD1306_DrawString5x7(6U, 54U, "BOOTING...");

	SSD1306_UpdateScreen();
}

static void OLED_ShowNetworkStatus(const AppNetworkStatus_t *pStatus, uint8_t phy_addr){
	char line[22];

	if(pStatus == 0U) return;

	SSD1306_Clear();

	SSD1306_DrawRect(0U, 0U, 128U, 64U, SSD1306_PIXEL_ON);

	SSD1306_DrawString5x7(4U, 3U, "NETWORK STATUS");

	snprintf(line, sizeof(line), "OLED:%s ETH:%s",
			pStatus->oled_ok ? "OK" : "--",
			pStatus->eth_reset_ok ? "OK" : "--");
	SSD1306_DrawString5x7(4U, 15U, line);

	snprintf(line, sizeof(line), "PHY:%u LINK:%s",
			phy_addr,
			pStatus->link_ok ? "OK" : "--");
	SSD1306_DrawString5x7(4U, 25U, line);

	snprintf(line, sizeof(line), "AUTO:%s DMA:%s",
			pStatus->autoneg_ok ? "OK" : "--",
			pStatus->mac_dma_ok ? "OK" : "--");
	SSD1306_DrawString5x7(4U, 35U, line);

	snprintf(line, sizeof(line), "ARP:%s PING:%s",
			pStatus->arp_ok ? "OK" : "--",
			pStatus->icmp_ok ? "OK" : "--");
	SSD1306_DrawString5x7(4U, 45U, line);

	snprintf(line, sizeof(line), "UDP:%s FORZA:%s",
			pStatus->udp_ok ? "OK" : "--",
			pStatus->forza_ok ? "OK" : "--");
	SSD1306_DrawString5x7(4U, 55U, line);

	SSD1306_UpdateScreen();
}

static void OLED_ShowWaitingForza(uint32_t udp_count, uint32_t forza_count){
	char line[22];

	SSD1306_Clear();

	SSD1306_DrawRect(0U, 0U, 128U, 64U, SSD1306_PIXEL_ON);

	SSD1306_DrawString5x7(12U, 8U, "WAIT FORZA UDP");

	snprintf(line, sizeof(line), "UDP  %lu", udp_count);
	SSD1306_DrawString5x7(12U, 26U, line);

	snprintf(line, sizeof(line), "FZDB %lu", forza_count);
	SSD1306_DrawString5x7(12U, 38U, line);

	SSD1306_DrawString5x7(12U, 52U, "DATA OUT: ON");

	SSD1306_UpdateScreen();
}

static void OLED_ShowForzaDashboard(const ForzaDB_Dashboard_t *pDash){
	char line[22];
	uint8_t rpm_percent;
	uint8_t steer_x;

	if(pDash == 0U) return;

	SSD1306_Clear();

	/*
	 * Main frame
	 */
	SSD1306_DrawRect(0U, 0U, 128U, 64U, SSD1306_PIXEL_ON);

	/*
	 * Speed and gear
	 */
	SSD1306_DrawString5x7(4U, 4U, "SPD");
	SSD1306_DrawNumberLarge(30U, 2U, pDash->SpeedKmh);

	snprintf(line, sizeof(line), "G%u", pDash->Gear);
	SSD1306_DrawString5x7(104U, 7U, line);

	/*
	 * RPM bar
	 */
	rpm_percent = 0U;

	if(pDash->MaxRpm > 0U){
		rpm_percent = (uint8_t)(((uint32_t)pDash->Rpm * 100U) / pDash->MaxRpm);
	}

	if(rpm_percent > 100U){
		rpm_percent = 100U;
	}

	snprintf(line, sizeof(line), "RPM %u", pDash->Rpm);
	SSD1306_DrawString5x7(4U, 27U, line);
	SSD1306_DrawHBar(58U, 26U, 64U, 8U, rpm_percent);

	/*
	 * Accel and brake bars
	 */
	SSD1306_DrawString5x7(4U, 39U, "THR");
	SSD1306_DrawHBar(28U, 38U, 38U, 7U, pDash->AccelPercent);

	SSD1306_DrawString5x7(70U, 39U, "BRK");
	SSD1306_DrawHBar(94U, 38U, 28U, 7U, pDash->BrakePercent);

	/*
	 * Steering marker
	 */
	SSD1306_DrawString5x7(4U, 51U, "STR");
	SSD1306_DrawRect(28U, 52U, 58U, 5U, SSD1306_PIXEL_ON);

	steer_x = (uint8_t)(56U + (((int16_t)pDash->Steer * 27) / 127));

	if(steer_x < 28U){
		steer_x = 28U;
	}

	if(steer_x > 86U){
		steer_x = 86U;
	}

	SSD1306_FillRect(steer_x, 50U, 3U, 9U, SSD1306_PIXEL_ON);

	/*
	 * Slip warning
	 */
	snprintf(line, sizeof(line), "S%u", pDash->SlipPercent);
	SSD1306_DrawString5x7(92U, 51U, line);

	if(pDash->WarningLevel != FORZADB_WARNING_NONE){
		SSD1306_FillRect(120U, 4U, 4U, 4U, SSD1306_PIXEL_ON);
	}

	SSD1306_UpdateScreen();
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

/******************************************************************************************************
 * 								Test Application
 ******************************************************************************************************/
volatile uint32_t mainloopalive = 0U;

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

	FPU_Enable();

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
	 * 1) OLED comes first.
	 *
	 * Why?
	 * If Ethernet init fails, we can still print the failure on screen.
	 */
	OLED_I2C_GPIO_Init();
	OLED_I2C_Init(&I2C1Handle);

	dbg_oled_status = SSD1306_Init(&I2C1Handle);

	if(dbg_oled_status == I2C_OK){
		network_status.oled_ok = 1U;
	}

	OLED_ShowStartupInfo();
	Delay_ms(OLED_STARTUP_INFO_TIME_MS);

	OLED_ShowNetworkStatus(&network_status, phy_addr);

	/*
	 * 2) Ethernet memory/cache configuration.
	 */
	dbg_init_step = 1U;
	ETH_MPU_ConfigNonCacheable();

	/*
	 * 3) Ethernet basic peripheral init.
	 */
	dbg_init_step = 2U;
	ETH_PeriClockControl(ENABLE);
	ETH_GPIO_RMII_Init();
	ETH_SelectRMII();

	/*
	 * 4) Ethernet software reset.
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
	 * 6) Wait for Ethernet link.
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

	/*
	 * 7) Wait for auto-negotiation.
	 */
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
	 * 8) Ethernet MAC / DMA setup.
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
	 * Promiscuous mode is useful for early testing.
	 * Later we can change this to destination-MAC based filtering.
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
		mainloopalive++;
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

				if(eth_frame.frame_format == ETHNET_FRAME_IEEE_802_3){
					dbg_ethnet_8023_count++;
				} else if(eth_frame.ethertype == ETHNET_ETHERTYPE_ARP){
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
						 *
						 * This keeps ping working while Forza UDP is running.
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

							if(ETH_SendRawFrame(icmp_reply_frame, icmp_reply_len) == 1U){
								dbg_icmp_reply_sent_count++;
								network_status.icmp_ok = 1U;
							} else{
								dbg_icmp_reply_fail_count++;
							}
						}

						/*
						 * UDP / Forza Data Out
						 *
						 * ETHNET_GetUDPPayload now includes IPv4/UDP length and
						 * checksum checks.
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
									 *
									 * Why?
									 * Forza can send many packets per second.
									 * I2C OLED full-screen update is much slower
									 * than memory variable update.
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
