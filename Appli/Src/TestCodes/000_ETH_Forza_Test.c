/*
 * 000_ETH_Forza_OLED_Test.c
 *
 *  Created on: 11 Haz 2026
 *      Author: talha
 */

#include <stdint.h>
#include <stdio.h>

#include "stm32h7sxx.h"
#include "stm32h7sxx_gpio_driver.h"
#include "stm32h7sxx_i2c_driver.h"
#include "stm32h7sxx_eth_driver.h"
#include "ssd1306.h"

/*
 * Forza UDP packet selection
 */
#define FORZA_PACKET_LEN             324U
#define FORZA_IS_RACE_ON_OFFSET      0U
#define FORZA_TIMESTAMP_OFFSET       4U
#define FORZA_ENGINE_MAX_RPM_OFFSET  8U
#define FORZA_RPM_OFFSET             16U
#define FORZA_CAR_ORDINAL_OFFSET     212U
#define FORZA_SPEED_OFFSET           256U
#define FORZA_FUEL_OFFSET            288U
#define FORZA_GEAR_OFFSET            319U

/*
 * UDP Port
 *
 * Forza Data Out Port should be set to this port.
 */
#define TEST_UDP_PORT                5005U

/*
 * OLED update selection
 */
#define OLED_STARTUP_INFO_TIME_MS    10000U
#define OLED_STATUS_TIME_MS          10000U
#define OLED_DASH_UPDATE_PERIOD_MS   100U

/*
 * Simple delay calibration.
 *
 * Bu değer tam gerçek ms olmayabilir.
 * Şu an amaç açılış ekranını yaklaşık 10 saniye tutmak.
 */
#define DELAY_MS_LOOP_COUNT          12000U

/*
 * Debug variables
 */
volatile uint8_t  dbg_init_ok = 0U;
volatile uint8_t  dbg_phy_addr = 0U;
volatile uint8_t  dbg_frame_available = 0U;
volatile uint8_t  dbg_udp_available = 0U;

volatile uint32_t dbg_payload_length = 0U;
volatile uint32_t dbg_forza_packet_count = 0U;
volatile uint32_t dbg_bad_len_count = 0U;

volatile uint16_t dbg_speed_kmh = 0U;
volatile uint16_t dbg_rpm = 0U;
volatile uint16_t dbg_rpm_max = 0U;
volatile uint8_t  dbg_rpm_percent = 0U;
volatile uint8_t  dbg_gear = 0U;
volatile uint8_t  dbg_fuel_percent = 0U;
volatile uint8_t  dbg_damage_percent = 0U;

volatile int32_t  dbg_is_race_on = 0;
volatile uint32_t dbg_timestamp_ms = 0U;
volatile int32_t  dbg_car_ordinal = 0;

volatile uint32_t dbg_error_code = 0U;
volatile uint32_t dbg_init_step = 0U;
volatile uint8_t  dbg_oled_status = 0U;

/*
 * FPU register
 */
#define SCB_CPACR_ADDR               (0xE000ED88UL)
#define SCB_CPACR                    (*(volatile uint32_t *)SCB_CPACR_ADDR)

/*
 * Forza dashboard data
 */
typedef struct{
	uint8_t valid;

	uint16_t speed_kmh;
	uint16_t rpm;
	uint16_t rpm_max;
	uint8_t rpm_percent;

	uint8_t gear;
	uint8_t fuel_percent;
	uint8_t damage_percent;

	int32_t car_ordinal;
	int32_t is_race_on;
	uint32_t timestamp_ms;
} ForzaDash_t;

/*
 * Private helper prototypes
 */
static void FPU_Enable(void);
static void Delay_ms(uint32_t ms);

static void OLED_I2C_GPIO_Init(void);
static void OLED_I2C_Init(I2C_Handle_t *pI2CHandle);

static uint32_t ReadU32LE(const uint8_t *data, uint32_t offset);
static int32_t ReadI32LE(const uint8_t *data, uint32_t offset);
static float ReadF32LE(const uint8_t *data, uint32_t offset);

static uint8_t Forza_ParsePayload(const uint8_t *payload,
                                  uint32_t payload_length,
                                  ForzaDash_t *out);

static void OLED_ShowStartupInfo(void);
static void OLED_ShowConnectionStatus(uint8_t phy_addr,
                                      uint32_t packet_count,
                                      uint8_t eth_ok);
static void OLED_ShowForzaDashboard(const ForzaDash_t *data);

/*
 * Private helper functions
 */
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

	while (ms > 0U){
		for (i = 0U; i < DELAY_MS_LOOP_COUNT; i++)
		{
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

static uint32_t ReadU32LE(const uint8_t *data, uint32_t offset){
	return ((uint32_t)data[offset])              |
	       ((uint32_t)data[offset + 1U] << 8U)  |
	       ((uint32_t)data[offset + 2U] << 16U) |
	       ((uint32_t)data[offset + 3U] << 24U);
}

static int32_t ReadI32LE(const uint8_t *data, uint32_t offset){
	return (int32_t)ReadU32LE(data, offset);
}

static float ReadF32LE(const uint8_t *data, uint32_t offset){
	union
	{
		uint32_t u32;
		float f32;
	} value;

	value.u32 = ReadU32LE(data, offset);

	return value.f32;
}

static uint8_t Forza_ParsePayload(const uint8_t *payload, uint32_t payload_length, ForzaDash_t *out){
	float rpm;
	float rpm_max;
	float speed_mps;
	float speed_kmh;
	float fuel;

	if ((payload == 0) || (out == 0)){
		return 0U;
	}

	if (payload_length != FORZA_PACKET_LEN){
		return 0U;
	}

	out->is_race_on = ReadI32LE(payload, FORZA_IS_RACE_ON_OFFSET);
	out->timestamp_ms = ReadU32LE(payload, FORZA_TIMESTAMP_OFFSET);

	rpm_max = ReadF32LE(payload, FORZA_ENGINE_MAX_RPM_OFFSET);
	rpm = ReadF32LE(payload, FORZA_RPM_OFFSET);

	speed_mps = ReadF32LE(payload, FORZA_SPEED_OFFSET);
	speed_kmh = speed_mps * 3.6f;

	fuel = ReadF32LE(payload, FORZA_FUEL_OFFSET);

	out->car_ordinal = ReadI32LE(payload, FORZA_CAR_ORDINAL_OFFSET);
	out->gear = payload[FORZA_GEAR_OFFSET];

	if (rpm < 0.0f){
		rpm = 0.0f;
	}

	if (rpm_max < 1.0f){
		rpm_max = 1.0f;
	}

	if (speed_kmh < 0.0f){
		speed_kmh = 0.0f;
	}

	if (fuel < 0.0f){
		fuel = 0.0f;
	}
	else if (fuel > 1.0f){
		fuel = 1.0f;
	}

	out->rpm = (uint16_t)(rpm + 0.5f);
	out->rpm_max = (uint16_t)(rpm_max + 0.5f);
	out->speed_kmh = (uint16_t)(speed_kmh + 0.5f);

	out->rpm_percent = (uint8_t)((rpm * 100.0f) / rpm_max);

	if (out->rpm_percent > 100U){
		out->rpm_percent = 100U;
	}

	out->fuel_percent = (uint8_t)(fuel * 100.0f);

	/*
	 * Current 324-byte parser does not include real damage field.
	 * Later we can replace this with real Forza damage offset if available.
	 */
	out->damage_percent = 0U;

	out->valid = 1U;

	return 1U;
}

static void OLED_ShowStartupInfo(void){
	SSD1306_Clear();

	SSD1306_DrawRect(0U, 0U, 128U, 64U, SSD1306_PIXEL_ON);

	SSD1306_DrawString5x7(6U, 6U,  "FORZA HUD");
	SSD1306_DrawString5x7(6U, 18U, "UDP 5005");
	SSD1306_DrawString5x7(6U, 30U, "BC 169.254.255.255");
	SSD1306_DrawString5x7(6U, 42U, "MAC 02-00-00-00-00-01");
	SSD1306_DrawString5x7(6U, 54U, "BOOT INFO");

	SSD1306_UpdateScreen();
}

static void OLED_ShowConnectionStatus(uint8_t phy_addr, uint32_t packet_count, uint8_t eth_ok){
	char line[22];

	SSD1306_Clear();

	SSD1306_DrawRect(0U, 0U, 128U, 64U, SSD1306_PIXEL_ON);

	if (eth_ok == 1U){
		SSD1306_DrawString5x7(6U, 6U, "ETH OK");
	} else{
		SSD1306_DrawString5x7(6U, 6U, "ETH ERR");
	}

	snprintf(line, sizeof(line), "PHY %u", phy_addr);
	SSD1306_DrawString5x7(6U, 18U, line);

	snprintf(line, sizeof(line), "UDP PORT %u", TEST_UDP_PORT);
	SSD1306_DrawString5x7(6U, 30U, line);

	snprintf(line, sizeof(line), "PKT %lu", packet_count);
	SSD1306_DrawString5x7(6U, 42U, line);

	SSD1306_DrawString5x7(6U, 54U, "WAIT FORZA UDP");

	SSD1306_UpdateScreen();
}

static void OLED_ShowForzaDashboard(const ForzaDash_t *data){
	char line[22];

	if ((data == 0) || (data->valid == 0U)){
		return;
	}

	SSD1306_Clear();

	/*
	 * Main frame
	 */
	SSD1306_DrawRect(0U, 0U, 128U, 64U, SSD1306_PIXEL_ON);

	/*
	 * RPM bar at top
	 */
	snprintf(line, sizeof(line), "RPM %u", data->rpm);
	SSD1306_DrawString5x7(4U, 4U, line);
	SSD1306_DrawHBar(62U, 4U, 60U, 8U, data->rpm_percent);

	/*
	 * Big speed number
	 */
	SSD1306_DrawNumberLarge(36U, 18U, data->speed_kmh);
	SSD1306_DrawString5x7(88U, 38U, "KMH");

	/*
	 * Bottom debug/status
	 */
	snprintf(line, sizeof(line), "G%u F%u%% D%u%%",
	         data->gear,
	         data->fuel_percent,
	         data->damage_percent);
	SSD1306_DrawString5x7(4U, 50U, line);

	SSD1306_UpdateScreen();
}

void Test_000_ETH_Forza_Test(void){
	I2C_Handle_t I2C1Handle;

	uint8_t phy_addr = 0U;
	uint8_t phy_found = 0U;
	uint8_t eth_reset_ok = 0U;

	uint8_t mac[ETH_MAC_ADDR_LEN] = {
			0x02U, 0x00U, 0x00U, 0x00U, 0x00U, 0x01U
	};

	uint8_t *frame = 0;
	uint32_t frame_length = 0U;
	uint8_t frame_available = 0U;

	uint8_t *payload = 0;
	uint32_t payload_length = 0U;
	uint8_t udp_available = 0U;

	ForzaDash_t forza;
	uint32_t last_oled_timestamp = 0U;

	FPU_Enable();

	forza.valid = 0U;

	/*
	 * 1) OLED comes first.
	 *
	 * Why?
	 * If Ethernet init fails, we can still print the failure on screen.
	 */
	OLED_I2C_GPIO_Init();
	OLED_I2C_Init(&I2C1Handle);

	dbg_oled_status = SSD1306_Init(&I2C1Handle);

	/*
	 * 2) First 10 seconds: device connection info.
	 */
	OLED_ShowStartupInfo();
	Delay_ms(OLED_STARTUP_INFO_TIME_MS);

	/*
	 * 3) Ethernet memory/cache configuration.
	 */
	dbg_init_step = 1U;
	ETH_MPU_ConfigNonCacheable();

	/*
	 * 4) Ethernet basic peripheral init.
	 */
	dbg_init_step = 2U;
	ETH_PeriClockControl(ENABLE);
	ETH_GPIO_RMII_Init();
	ETH_SelectRMII();

	/*
	 * 5) Ethernet software reset.
	 */
	dbg_init_step = 3U;
	eth_reset_ok = ETH_SoftwareReset();

	if (eth_reset_ok == 0U){
		dbg_error_code = 1U;
		dbg_init_ok = 0U;

		SSD1306_Clear();
		SSD1306_DrawString5x7(6U, 6U, "ETH RESET ERR");
		SSD1306_UpdateScreen();

		while(1);
	}

	/*
	 * 6) PHY address scan.
	 */
	dbg_init_step = 4U;
	phy_found = ETH_PHY_FindAddress(&phy_addr);

	if (phy_found == 0U){
		dbg_error_code = 2U;
		dbg_init_ok = 0U;

		SSD1306_Clear();
		SSD1306_DrawString5x7(6U, 6U, "PHY NOT FOUND");
		SSD1306_UpdateScreen();

		while(1);
	}

	dbg_phy_addr = phy_addr;

	/*
	 * 7) Wait for Ethernet link.
	 */
	dbg_init_step = 5U;
	if (ETH_PHY_WaitForLink(phy_addr, ETH_TIMEOUT_VALUE) == 0U){
		dbg_error_code = 3U;
		dbg_init_ok = 0U;

		SSD1306_Clear();
		SSD1306_DrawString5x7(6U, 6U, "LINK ERR");
		SSD1306_UpdateScreen();

		while(1);
	}

	/*
	 * 8) Wait for auto-negotiation.
	 */
	dbg_init_step = 6U;
	if (ETH_PHY_WaitAutoNegotiation(phy_addr, ETH_TIMEOUT_VALUE) == 0U){
		dbg_error_code = 4U;
		dbg_init_ok = 0U;

		SSD1306_Clear();
		SSD1306_DrawString5x7(6U, 6U, "AUTO NEG ERR");
		SSD1306_UpdateScreen();

		while(1);
	}

	/*
	 * 9) Second 10 seconds: connection status.
	 */
	OLED_ShowConnectionStatus(phy_addr, 0U, 1U);
	Delay_ms(OLED_STATUS_TIME_MS);

	/*
	 * 10) Ethernet MAC / DMA setup.
	 */
	dbg_init_step = 7U;
	ETH_SetMACAddress(mac);

	dbg_init_step = 8U;
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
	 * PM = promiscuous mode.
	 * Why?
	 * We do not have a real IP stack yet. We want to accept broadcast/raw frames.
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

	/*
	 * First runtime screen.
	 */
	OLED_ShowConnectionStatus(phy_addr, 0U, 1U);

	while(1){
		frame = 0;
		frame_length = 0U;
		payload = 0;
		payload_length = 0U;
		udp_available = 0U;

		frame_available = ETH_ReadRawFrame(&frame, &frame_length);

		dbg_frame_available = frame_available;

		if (frame_available == 1U){
			udp_available = ETH_GetUDPPayload(frame,
			                                  frame_length,
			                                  TEST_UDP_PORT,
			                                  &payload,
			                                  &payload_length);

			dbg_udp_available = udp_available;

			if (udp_available == 1U){
				dbg_payload_length = payload_length;

				if (Forza_ParsePayload(payload, payload_length, &forza) == 1U){
					dbg_forza_packet_count++;

					dbg_is_race_on = forza.is_race_on;
					dbg_timestamp_ms = forza.timestamp_ms;

					dbg_speed_kmh = forza.speed_kmh;
					dbg_rpm = forza.rpm;
					dbg_rpm_max = forza.rpm_max;
					dbg_rpm_percent = forza.rpm_percent;
					dbg_gear = forza.gear;
					dbg_fuel_percent = forza.fuel_percent;
					dbg_damage_percent = forza.damage_percent;
					dbg_car_ordinal = forza.car_ordinal;

					/*
					 * OLED update is limited by Forza timestamp.
					 *
					 * Why?
					 * Forza can send many packets per second.
					 * I2C OLED full-screen update is much slower than memory variable update.
					 */
					if ((forza.timestamp_ms - last_oled_timestamp) >= OLED_DASH_UPDATE_PERIOD_MS){
						last_oled_timestamp = forza.timestamp_ms;
						OLED_ShowForzaDashboard(&forza);
					}
				} else{
					dbg_bad_len_count++;

					/*
					 * If wrong UDP payload comes, show status screen occasionally.
					 */
					OLED_ShowConnectionStatus(phy_addr,
					                          dbg_forza_packet_count,
					                          1U);
				}
			}

			ETH_ReleaseRXDescriptor();
		}
	}
}
