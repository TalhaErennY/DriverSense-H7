/*
 * ForzaDB.c
 *
 *  Created on: 11 Haz 2026
 *      Author: talha
 */

#include <stdio.h>
#include "ForzaDB.h"
#include "stm32h7sxx_gpio_driver.h"
#include "stm32h7sxx_i2c_driver.h"
#include "ssd1306.h"

/******************************************************************************************************
 * 								Private Helper Prototypes
 ******************************************************************************************************/
static uint16_t ReadU16LE(const uint8_t *pData, uint32_t offset);
static uint32_t ReadU32LE(const uint8_t *pData, uint32_t offset);
static int32_t  ReadS32LE(const uint8_t *pData, uint32_t offset);
static float    ReadF32LE(const uint8_t *pData, uint32_t offset);

static uint8_t ScaleU8ToPercent(uint8_t value);
static uint8_t ClampFloatToPercent(float value);
static uint8_t AbsMax4ToPercent(float a, float b, float c, float d);
static float Max4Float(float a, float b, float c, float d);

/*
 * OLED dashboard drawing helpers
 */
typedef struct{
	uint8_t x_in;
	uint8_t y_in;
	uint8_t x_out;
	uint8_t y_out;
} OLED_RpmTick_t;

static uint8_t OLED_ClampPercent(uint8_t percent);
static uint8_t OLED_CalcRpmPercent(uint16_t rpm, uint16_t max_rpm);
static uint8_t OLED_GetLargeNumberX(uint16_t value, uint8_t center_x);

static void OLED_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
static void OLED_DrawMiniSegmentBar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t percent);
static void OLED_DrawGearStrip(uint8_t x, uint8_t y, uint8_t gear);
static void OLED_DrawRpmArcMath(uint8_t rpm_percent, uint16_t max_rpm);
static void OLED_DrawShiftBoxSmall(uint8_t x, uint8_t y, uint8_t is_up, uint8_t active);
static void OLED_DrawBottomStatusClean(const ForzaDB_Dashboard_t *pDash);

/******************************************************************************************************
 * 								Forza Packet Parser Functions
 ******************************************************************************************************/
/****************************************************************************
 * @fn				- ForzaDB_ParsePacket
 *
 * @brief			- This function parses a Forza Horizon Data Out UDP payload
 *
 * @param[in]		- pointer to UDP payload buffer
 * @param[in]		- UDP payload length
 * @param[in]		- pointer to parsed Forza packet structure
 *
 * @return			- 1 if packet is parsed successfully, 0 otherwise
 *
 * @Note			- Forza Data Out packet size must be 324 bytes.
 * 					- Multi-byte fields are little-endian.
 *					- The last byte is treated as reserved/padding.
 *
 */
uint8_t ForzaDB_ParsePacket(const uint8_t *pPayload, uint32_t PayloadLen, ForzaDB_Packet_t *pOut){
	if((pPayload == 0U) || (pOut == 0U)) return 0U;
	if(PayloadLen != FORZADB_PACKET_SIZE) return 0U;

	/*
	 * Race and time
	 */
	pOut->IsRaceOn   = ReadS32LE(pPayload, FORZADB_OFF_ISRACEON);
	pOut->TimestampMS = ReadU32LE(pPayload, FORZADB_OFF_TIMESTAMP_MS);

	/*
	 * Engine
	 */
	pOut->EngineMaxRpm     = ReadF32LE(pPayload, FORZADB_OFF_ENGINE_MAX_RPM);
	pOut->EngineIdleRpm    = ReadF32LE(pPayload, FORZADB_OFF_ENGINE_IDLE_RPM);
	pOut->CurrentEngineRpm = ReadF32LE(pPayload, FORZADB_OFF_ENGINE_CURRENT_RPM);

	/*
	 * Acceleration
	 */
	pOut->AccelerationX = ReadF32LE(pPayload, FORZADB_OFF_ACCELERATION_X);
	pOut->AccelerationY = ReadF32LE(pPayload, FORZADB_OFF_ACCELERATION_Y);
	pOut->AccelerationZ = ReadF32LE(pPayload, FORZADB_OFF_ACCELERATION_Z);

	/*
	 * Velocity
	 */
	pOut->VelocityX = ReadF32LE(pPayload, FORZADB_OFF_VELOCITY_X);
	pOut->VelocityY = ReadF32LE(pPayload, FORZADB_OFF_VELOCITY_Y);
	pOut->VelocityZ = ReadF32LE(pPayload, FORZADB_OFF_VELOCITY_Z);

	/*
	 * Angular velocity
	 */
	pOut->AngularVelocityX = ReadF32LE(pPayload, FORZADB_OFF_ANGULAR_VELOCITY_X);
	pOut->AngularVelocityY = ReadF32LE(pPayload, FORZADB_OFF_ANGULAR_VELOCITY_Y);
	pOut->AngularVelocityZ = ReadF32LE(pPayload, FORZADB_OFF_ANGULAR_VELOCITY_Z);

	/*
	 * Orientation
	 */
	pOut->Yaw   = ReadF32LE(pPayload, FORZADB_OFF_YAW);
	pOut->Pitch = ReadF32LE(pPayload, FORZADB_OFF_PITCH);
	pOut->Roll  = ReadF32LE(pPayload, FORZADB_OFF_ROLL);

	/*
	 * Tire slip ratio
	 */
	pOut->TireSlipRatioFrontLeft  = ReadF32LE(pPayload, FORZADB_OFF_TIRE_SLIP_RATIO_FL);
	pOut->TireSlipRatioFrontRight = ReadF32LE(pPayload, FORZADB_OFF_TIRE_SLIP_RATIO_FR);
	pOut->TireSlipRatioRearLeft   = ReadF32LE(pPayload, FORZADB_OFF_TIRE_SLIP_RATIO_RL);
	pOut->TireSlipRatioRearRight  = ReadF32LE(pPayload, FORZADB_OFF_TIRE_SLIP_RATIO_RR);

	/*
	 * Tire slip angle
	 */
	pOut->TireSlipAngleFrontLeft  = ReadF32LE(pPayload, FORZADB_OFF_TIRE_SLIP_ANGLE_FL);
	pOut->TireSlipAngleFrontRight = ReadF32LE(pPayload, FORZADB_OFF_TIRE_SLIP_ANGLE_FR);
	pOut->TireSlipAngleRearLeft   = ReadF32LE(pPayload, FORZADB_OFF_TIRE_SLIP_ANGLE_RL);
	pOut->TireSlipAngleRearRight  = ReadF32LE(pPayload, FORZADB_OFF_TIRE_SLIP_ANGLE_RR);

	/*
	 * Tire combined slip
	 */
	pOut->TireCombinedSlipFrontLeft  = ReadF32LE(pPayload, FORZADB_OFF_TIRE_COMBINED_SLIP_FL);
	pOut->TireCombinedSlipFrontRight = ReadF32LE(pPayload, FORZADB_OFF_TIRE_COMBINED_SLIP_FR);
	pOut->TireCombinedSlipRearLeft   = ReadF32LE(pPayload, FORZADB_OFF_TIRE_COMBINED_SLIP_RL);
	pOut->TireCombinedSlipRearRight  = ReadF32LE(pPayload, FORZADB_OFF_TIRE_COMBINED_SLIP_RR);

	/*
	 * Vehicle output
	 */
	pOut->Speed  = ReadF32LE(pPayload, FORZADB_OFF_SPEED);
	pOut->Power  = ReadF32LE(pPayload, FORZADB_OFF_POWER);
	pOut->Torque = ReadF32LE(pPayload, FORZADB_OFF_TORQUE);

	/*
	 * Tire temperature
	 */
	pOut->TireTempFrontLeft  = ReadF32LE(pPayload, FORZADB_OFF_TIRE_TEMP_FL);
	pOut->TireTempFrontRight = ReadF32LE(pPayload, FORZADB_OFF_TIRE_TEMP_FR);
	pOut->TireTempRearLeft   = ReadF32LE(pPayload, FORZADB_OFF_TIRE_TEMP_RL);
	pOut->TireTempRearRight  = ReadF32LE(pPayload, FORZADB_OFF_TIRE_TEMP_RR);

	/*
	 * Boost, fuel and distance
	 */
	pOut->Boost            = ReadF32LE(pPayload, FORZADB_OFF_BOOST);
	pOut->Fuel             = ReadF32LE(pPayload, FORZADB_OFF_FUEL);
	pOut->DistanceTraveled = ReadF32LE(pPayload, FORZADB_OFF_DISTANCE_TRAVELED);


	/*
	 * Lap and race time
	 */
	pOut->BestLap         = ReadF32LE(pPayload, FORZADB_OFF_BEST_LAP);
	pOut->LastLap         = ReadF32LE(pPayload, FORZADB_OFF_LAST_LAP);
	pOut->CurrentLap      = ReadF32LE(pPayload, FORZADB_OFF_CURRENT_LAP);
	pOut->CurrentRaceTime = ReadF32LE(pPayload, FORZADB_OFF_CURRENT_RACE_TIME);
	pOut->LapNumber    = ReadU16LE(pPayload, FORZADB_OFF_LAP_NUMBER);
	pOut->RacePosition = pPayload[FORZADB_OFF_RACE_POSITION];

	/*
	 * Player inputs
	 */
	pOut->Accel     = pPayload[FORZADB_OFF_ACCEL];
	pOut->Brake     = pPayload[FORZADB_OFF_BRAKE];
	pOut->Clutch    = pPayload[FORZADB_OFF_CLUTCH];
	pOut->HandBrake = pPayload[FORZADB_OFF_HANDBRAKE];
	pOut->Gear      = pPayload[FORZADB_OFF_GEAR];

	pOut->Steer                       = (int8_t)pPayload[FORZADB_OFF_STEER];
	pOut->NormalizedDrivingLine       = (int8_t)pPayload[FORZADB_OFF_NORMALIZED_DRIVING_LINE];
	pOut->NormalizedAIBrakeDifference = (int8_t)pPayload[FORZADB_OFF_NORMALIZED_AI_BRAKE_DIFF];

	return 1U;
}

/****************************************************************************
 * @fn				- ForzaDB_BuildDashboardData
 *
 * @brief			- This function converts parsed Forza packet data into
 * 					  compact dashboard data
 *
 * @param[in]		- pointer to parsed Forza packet structure
 * @param[in]		- pointer to dashboard data structure
 * @param[in]		-
 *
 * @return			- none
 *
 * @Note			- Speed is converted from m/s to km/h.
 * 					- Accel, brake, clutch and handbrake are converted to percent.
 *					- Tire combined slip is converted to simple warning level.
 *
 */
void ForzaDB_BuildDashboardData(const ForzaDB_Packet_t *pPacket, ForzaDB_Dashboard_t *pDash){
	float speed_kmh;
	uint8_t slip_percent;
	static uint16_t last_valid_max_rpm = 8000U;

	if((pPacket == 0U) || (pDash == 0U)) return;

	/*
	 * Speed conversion: m/s -> km/h
	 */
	speed_kmh = pPacket->Speed * 3.6f;

	if(speed_kmh < 0.0f){
		speed_kmh = 0.0f;
	}

	if(speed_kmh > 999.0f){
		speed_kmh = 999.0f;
	}

	pDash->SpeedKmh = (uint16_t)speed_kmh;

	/*
	 * RPM values
	 */
	if(pPacket->CurrentEngineRpm < 0.0f){
		pDash->Rpm = 0U;
	} else{
		pDash->Rpm = (uint16_t)pPacket->CurrentEngineRpm;
	}

	/*
	 * Max RPM can briefly become 0 or invalid when the car changes,
	 * when the game is in menu or while telemetry is being refreshed.
	 *
	 * Keep the last valid MaxRpm so the RPM scale does not collapse
	 * during vehicle changes. When a new valid car value arrives, the
	 * scale is updated automatically.
	 */
	if((pPacket->EngineMaxRpm >= 1000.0f) && (pPacket->EngineMaxRpm <= 20000.0f)){
		last_valid_max_rpm = (uint16_t)pPacket->EngineMaxRpm;
	}

	pDash->MaxRpm = last_valid_max_rpm;

	/*
	 * Driver inputs
	 */
	pDash->Gear             = pPacket->Gear;
	pDash->AccelPercent     = ScaleU8ToPercent(pPacket->Accel);
	pDash->BrakePercent     = ScaleU8ToPercent(pPacket->Brake);
	pDash->ClutchPercent    = ScaleU8ToPercent(pPacket->Clutch);
	pDash->HandBrakePercent = ScaleU8ToPercent(pPacket->HandBrake);

	pDash->Steer = pPacket->Steer;

	/*
	 * Slip, fuel and boost
	 */
	slip_percent = AbsMax4ToPercent(
			pPacket->TireCombinedSlipFrontLeft,
			pPacket->TireCombinedSlipFrontRight,
			pPacket->TireCombinedSlipRearLeft,
			pPacket->TireCombinedSlipRearRight
	);

	pDash->SlipPercent = slip_percent;
	pDash->FuelPercent = ClampFloatToPercent(pPacket->Fuel * 100.0f);

	/*
	 * Forza boost is PSI above atmospheric.
	 *
	 * BoostPercent is only a visual bar scale.
	 * BoostBarX10 is used to print bar value on OLED.
	 *
	 * bar = psi * 0.0689476
	 * bar_x10 = psi * 0.689476
	 */
	pDash->BoostPercent = ClampFloatToPercent(pPacket->Boost * 5.0f);

	if(pPacket->Boost <= 0.0f){
		pDash->BoostBarX10 = 0U;
	} else if(pPacket->Boost > 999.0f){
		pDash->BoostBarX10 = 999U;
	} else{
		pDash->BoostBarX10 = (uint16_t)(pPacket->Boost * 0.689476f);
	}

	/*
	 * DistanceTraveled is in meters.
	 *
	 * km_x10 = meters / 100
	 */
	if(pPacket->DistanceTraveled <= 0.0f){
		pDash->DistanceKmX10 = 0U;
	} else if(pPacket->DistanceTraveled > 6553500.0f){
		pDash->DistanceKmX10 = 65535U;
	} else{
		pDash->DistanceKmX10 = (uint16_t)(pPacket->DistanceTraveled / 100.0f);
	}

	/*
	 * Max tire tempreture
	 */
	float max_tire_temp;

	max_tire_temp = Max4Float(
			pPacket->TireTempFrontLeft,
			pPacket->TireTempFrontRight,
			pPacket->TireTempRearLeft,
			pPacket->TireTempRearRight
	);

	if(max_tire_temp <= 0.0f){
		pDash->TireTempC = 0U;
	} else if(max_tire_temp >= 255.0f){
		pDash->TireTempC = 255U;
	} else{
		pDash->TireTempC = (uint8_t)max_tire_temp;
	}

	/*
	 * Simple first warning logic
	 */
	if(slip_percent > 70U){
		pDash->WarningLevel = FORZADB_WARNING_HIGH;
	} else if(slip_percent > 40U){
		pDash->WarningLevel = FORZADB_WARNING_LOW;
	} else{
		pDash->WarningLevel = FORZADB_WARNING_NONE;
	}
}

/******************************************************************************************************
 * 								Forza OLED Display Functions
 ******************************************************************************************************/
/*
 * OLED I2C GPIO init
 *
 * Assumption:
 * I2C1 SCL -> PB8
 * I2C1 SDA -> PB9
 * AF4
 */
void OLED_I2C_GPIO_Init(void){
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

void OLED_I2C_Init(I2C_Handle_t *pI2CHandle){
	pI2CHandle->pI2Cx = I2C1;

	pI2CHandle->I2C_Config.I2C_Timing = I2C_TIMING_400KHZ;
	pI2CHandle->I2C_Config.I2C_DeviceAddress = 0U;
	pI2CHandle->I2C_Config.I2C_AddressingMode = I2C_ADDRMODE_7BIT;
	pI2CHandle->I2C_Config.I2C_NoStretchMode = I2C_STRETCH_ENABLE;

	I2C_Init(pI2CHandle);
}

void OLED_ShowStartupInfo(void){
	SSD1306_Clear();

	SSD1306_DrawRect(0U, 0U, 128U, 64U, SSD1306_PIXEL_ON);

	SSD1306_DrawString5x7(6U, 6U,  "DRIVESENSE-H7");
	SSD1306_DrawString5x7(6U, 18U, "FORZA DASHBOARD");
	SSD1306_DrawString5x7(6U, 30U, "IP 192.168.1.50");
	SSD1306_DrawString5x7(6U, 42U, "PORT 5005");
	SSD1306_DrawString5x7(6U, 54U, "BOOTING...");

	SSD1306_UpdateScreen();
}

void OLED_ShowNetworkStatus(const AppNetworkStatus_t *pStatus, uint8_t phy_addr){
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

void OLED_ShowWaitingForza(uint32_t udp_count, uint32_t forza_count){
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

void OLED_ShowForzaDashboard(const ForzaDB_Dashboard_t *pDash){
	char line[22];

	uint8_t rpm_percent;
	uint8_t speed_x;

	uint8_t shift_up_active = 0U;
	uint8_t shift_down_active = 0U;

	if(pDash == 0U) return;

	rpm_percent = OLED_CalcRpmPercent(pDash->Rpm, pDash->MaxRpm);

	/*
	 * Simple shift indicator logic.
	 */
	if(rpm_percent >= 85U){
		shift_up_active = 1U;
	} else if((rpm_percent <= 25U) && (pDash->SpeedKmh > 10U)){
		shift_down_active = 1U;
	}

	SSD1306_Clear();

	/*
	 * ============================================================
	 * LEFT SIDE - COMPACT TURBO
	 * ============================================================
	 */
	SSD1306_DrawString5x7(0U, 0U, "T");
	OLED_DrawMiniSegmentBar(1U, 8U, 7U, 20U, pDash->BoostPercent);

	snprintf(line, sizeof(line), "%u.%u",
			(uint16_t)(pDash->BoostBarX10 / 10U),
			(uint16_t)(pDash->BoostBarX10 % 10U));
	SSD1306_DrawString5x7(10U, 12U, line);
	SSD1306_DrawString5x7(10U, 21U, "B");

	OLED_DrawLine(0U, 31U, 24U, 31U);

	/*
	 * ============================================================
	 * LEFT SIDE - COMPACT FUEL
	 * ============================================================
	 */
	SSD1306_DrawString5x7(0U, 34U, "F");
	OLED_DrawMiniSegmentBar(1U, 42U, 7U, 12U, pDash->FuelPercent);

	SSD1306_DrawString5x7(10U, 40U, "F");
	SSD1306_DrawString5x7(10U, 49U, "E");

	/*
	 * ============================================================
	 * TOP CENTER - CURRENT GEAR
	 * ============================================================
	 *
	 * Only the selected gear is shown here.
	 */
	OLED_DrawGearStrip(60U, 0U, pDash->Gear);

	/*
	 * ============================================================
	 * TOP RIGHT - DISTANCE
	 * ============================================================
	 */
	SSD1306_DrawString5x7(97U, 0U, "DIST");

	snprintf(line, sizeof(line), "%u.%uK",
			(uint16_t)(pDash->DistanceKmX10 / 10U),
			(uint16_t)(pDash->DistanceKmX10 % 10U));
	SSD1306_DrawString5x7(98U, 8U, line);

	/*
	 * ============================================================
	 * CENTER - RPM ARC
	 * ============================================================
	 */
	OLED_DrawRpmArcMath(rpm_percent, pDash->MaxRpm);

	/*
	 * ============================================================
	 * CENTER - SPEED
	 * ============================================================
	 */
	speed_x = OLED_GetLargeNumberX(pDash->SpeedKmh, 66U);

	SSD1306_DrawNumberLarge(speed_x, 25U, pDash->SpeedKmh);
	SSD1306_DrawString5x7(92U, 45U, "KMH");

	/*
	 * ============================================================
	 * RIGHT SIDE - SHIFT BOXES
	 * ============================================================
	 *
	 * Only arrows are drawn. When a shift suggestion is active, the
	 * box is drawn in reverse style.
	 */
	OLED_DrawShiftBoxSmall(112U, 18U, 1U, shift_up_active);
	OLED_DrawShiftBoxSmall(112U, 38U, 0U, shift_down_active);

	/*
	 * ============================================================
	 * BOTTOM STATUS BAR
	 * ============================================================
	 */
	OLED_DrawBottomStatusClean(pDash);

	SSD1306_UpdateScreen();
}

/******************************************************************************************************
 * 								Private Helper Functions
 ******************************************************************************************************/
/*
 * Clamp percent value to 0-100 range.
 */
static uint8_t OLED_ClampPercent(uint8_t percent){
	if(percent > 100U){
		percent = 100U;
	}

	return percent;
}

/*
 * Calculate RPM percent from current RPM and maximum RPM.
 */
static uint8_t OLED_CalcRpmPercent(uint16_t rpm, uint16_t max_rpm){
	uint32_t percent;

	if(max_rpm == 0U){
		return 0U;
	}

	percent = ((uint32_t)rpm * 100U) / (uint32_t)max_rpm;

	if(percent > 100U){
		percent = 100U;
	}

	return (uint8_t)percent;
}

/*
 * Calculate x position to center large number.
 *
 * SSD1306_DrawNumberLarge uses:
 * - each digit area approximately 14 px visible
 * - cursor advances by 17 px
 */
static uint8_t OLED_GetLargeNumberX(uint16_t value, uint8_t center_x){
	uint8_t digit_count;
	uint8_t width;

	if(value > 999U){
		value = 999U;
	}

	if(value >= 100U){
		digit_count = 3U;
	} else if(value >= 10U){
		digit_count = 2U;
	} else{
		digit_count = 1U;
	}

	width = (uint8_t)((digit_count * 14U) + ((digit_count - 1U) * 3U));

	if(center_x < (width / 2U)){
		return 0U;
	}

	return (uint8_t)(center_x - (width / 2U));
}

/*
 * Draw line with integer Bresenham algorithm.
 *
 * This is used for RPM arc ticks.
 */
static void OLED_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1){
	int16_t dx;
	int16_t dy;
	int16_t sx;
	int16_t sy;
	int16_t err;
	int16_t e2;

	dx = (x0 > x1) ? (int16_t)(x0 - x1) : (int16_t)(x1 - x0);
	dy = (y0 > y1) ? (int16_t)(y0 - y1) : (int16_t)(y1 - y0);

	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;

	err = dx - dy;

	while(1){
		SSD1306_DrawPixel(x0, y0, SSD1306_PIXEL_ON);

		if((x0 == x1) && (y0 == y1)){
			break;
		}

		e2 = (int16_t)(2 * err);

		if(e2 > -dy){
			err = (int16_t)(err - dy);
			x0 = (uint8_t)((int16_t)x0 + sx);
		}

		if(e2 < dx){
			err = (int16_t)(err + dx);
			y0 = (uint8_t)((int16_t)y0 + sy);
		}
	}
}

/*
 * Draw segmented vertical bar.
 *
 * Used for:
 * - Turbo boost
 * - Fuel level
 *
 * The bar is filled from bottom to top.
 */
static void OLED_DrawMiniSegmentBar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t percent){
	uint8_t i;
	uint8_t seg_count;
	uint8_t filled;
	uint8_t seg_h;
	uint8_t seg_y;
	uint8_t inner_w;
	uint8_t step;

	percent = OLED_ClampPercent(percent);

	/*
	 * The same helper is used for turbo and fuel.
	 * A small bar must stay thin, so segment count depends on height.
	 */
	seg_count = (h >= 20U) ? 5U : 4U;
	seg_h = 1U;
	step = 3U;

	filled = (uint8_t)(((uint16_t)percent * seg_count + 99U) / 100U);

	SSD1306_DrawRect(x, y, w, h, SSD1306_PIXEL_ON);

	inner_w = (w > 3U) ? (uint8_t)(w - 3U) : 1U;

	for(i = 0U; i < seg_count; i++){
		seg_y = (uint8_t)(y + h - 3U - (i * step));

		if(i < filled){
			SSD1306_FillRect((uint8_t)(x + 2U),
							 seg_y,
							 inner_w,
							 seg_h,
							 SSD1306_PIXEL_ON);
		} else{
			OLED_DrawLine((uint8_t)(x + 2U),
						  seg_y,
						  (uint8_t)(x + 1U + inner_w),
						  seg_y);
		}
	}
}

/*
 * Draw compact current gear indicator.
 *
 * Only the selected gear is printed.
 * No gear strip and no underline are used.
 */
static void OLED_DrawGearStrip(uint8_t x, uint8_t y, uint8_t gear){
	char gear_text[4];

	if(gear == 255U){
		SSD1306_DrawString5x7(x, y, "R");
	} else if(gear == 0U){
		SSD1306_DrawString5x7(x, y, "N");
	} else{
		snprintf(gear_text, sizeof(gear_text), "%u", gear);
		SSD1306_DrawString5x7(x, y, gear_text);
	}
}

/*
 * Draw RPM arc using pre-calculated mathematical tick coordinates.
 *
 * No float/sin/cos is used at runtime.
 */
static void OLED_DrawRpmArcMath(uint8_t rpm_percent, uint16_t max_rpm){
	uint8_t i;
	uint8_t filled;
	uint16_t rpm_label_0;
	uint16_t rpm_label_25;
	uint16_t rpm_label_50;
	uint16_t rpm_label_75;
	uint16_t rpm_label_100;
	char label[4];

	static const OLED_RpmTick_t ticks[13] =
	{
		{ 29U, 43U, 25U, 40U },
		{ 32U, 36U, 28U, 33U },
		{ 37U, 30U, 32U, 27U },
		{ 45U, 25U, 40U, 23U },
		{ 54U, 22U, 49U, 20U },
		{ 62U, 20U, 62U, 16U },
		{ 70U, 20U, 70U, 16U },
		{ 78U, 22U, 83U, 20U },
		{ 87U, 25U, 92U, 23U },
		{ 94U, 30U, 99U, 27U },
		{ 99U, 36U, 103U, 33U },
		{ 102U, 43U, 106U, 40U },
		{ 104U, 47U, 108U, 46U }
	};

	rpm_percent = OLED_ClampPercent(rpm_percent);

	filled = (uint8_t)(((uint16_t)rpm_percent * 13U + 99U) / 100U);

	for(i = 0U; i < 13U; i++){
		if(i < filled){
			OLED_DrawLine(ticks[i].x_in,
						  ticks[i].y_in,
						  ticks[i].x_out,
						  ticks[i].y_out);

			/*
			 * Thickness effect.
			 */
			OLED_DrawLine((uint8_t)(ticks[i].x_in + 1U),
						  ticks[i].y_in,
						  (uint8_t)(ticks[i].x_out + 1U),
						  ticks[i].y_out);
		} else{
			SSD1306_DrawPixel(ticks[i].x_out,
							  ticks[i].y_out,
							  SSD1306_PIXEL_ON);
		}
	}

	/*
	 * RPM scale labels.
	 *
	 * The arc is scaled according to MaxRpm.
	 * Labels are displayed in x1000 RPM.
	 *
	 * Example:
	 * MaxRpm = 8000 -> 0, 2, 4, 6, 8
	 * MaxRpm = 9000 -> 0, 2, 5, 7, 9
	 */
	if(max_rpm == 0U){
		max_rpm = 8000U;
	}

	rpm_label_0 = 0U;
	rpm_label_25 = (uint16_t)(((uint32_t)max_rpm * 1U + 2000U) / 4000U);
	rpm_label_50 = (uint16_t)(((uint32_t)max_rpm * 2U + 2000U) / 4000U);
	rpm_label_75 = (uint16_t)(((uint32_t)max_rpm * 3U + 2000U) / 4000U);
	rpm_label_100 = (uint16_t)(((uint32_t)max_rpm + 500U) / 1000U);

	snprintf(label, sizeof(label), "%u", rpm_label_0);
	SSD1306_DrawString5x7(25U, 45U, label);

	snprintf(label, sizeof(label), "%u", rpm_label_25);
	SSD1306_DrawString5x7(36U, 20U, label);

	snprintf(label, sizeof(label), "%u", rpm_label_50);
	SSD1306_DrawString5x7(64U, 12U, label);

	snprintf(label, sizeof(label), "%u", rpm_label_75);
	SSD1306_DrawString5x7(91U, 20U, label);

	snprintf(label, sizeof(label), "%u", rpm_label_100);
	SSD1306_DrawString5x7(104U, 45U, label);
}

/*
 * Draw compact shift indicator box.
 */
static void OLED_DrawShiftBoxSmall(uint8_t x, uint8_t y, uint8_t is_up, uint8_t active){
	uint8_t arrow_state;

	/*
	 * Passive state:
	 * - empty box
	 * - bright arrow
	 *
	 * Active state:
	 * - filled box
	 * - dark arrow, reverse color effect
	 *
	 * The box is kept narrow and away from the far-right column.
	 * This prevents the single broken-looking pixel column on the
	 * right edge and keeps the shift indicators outside the RPM arc.
	 */
	if(active != 0U){
		SSD1306_FillRect(x, y, 12U, 16U, SSD1306_PIXEL_ON);
		arrow_state = SSD1306_PIXEL_OFF;
	} else{
		SSD1306_DrawRect(x, y, 12U, 16U, SSD1306_PIXEL_ON);
		arrow_state = SSD1306_PIXEL_ON;
	}

	if(is_up != 0U){
		/* Up arrow only */
		SSD1306_FillRect((uint8_t)(x + 5U), (uint8_t)(y + 5U), 2U, 7U, arrow_state);
		SSD1306_FillRect((uint8_t)(x + 3U), (uint8_t)(y + 5U), 6U, 2U, arrow_state);
		SSD1306_DrawPixel((uint8_t)(x + 4U), (uint8_t)(y + 4U), arrow_state);
		SSD1306_DrawPixel((uint8_t)(x + 7U), (uint8_t)(y + 4U), arrow_state);
		SSD1306_DrawPixel((uint8_t)(x + 5U), (uint8_t)(y + 3U), arrow_state);
		SSD1306_DrawPixel((uint8_t)(x + 6U), (uint8_t)(y + 3U), arrow_state);
	} else{
		/* Down arrow only */
		SSD1306_FillRect((uint8_t)(x + 5U), (uint8_t)(y + 4U), 2U, 7U, arrow_state);
		SSD1306_FillRect((uint8_t)(x + 3U), (uint8_t)(y + 9U), 6U, 2U, arrow_state);
		SSD1306_DrawPixel((uint8_t)(x + 4U), (uint8_t)(y + 11U), arrow_state);
		SSD1306_DrawPixel((uint8_t)(x + 7U), (uint8_t)(y + 11U), arrow_state);
		SSD1306_DrawPixel((uint8_t)(x + 5U), (uint8_t)(y + 12U), arrow_state);
		SSD1306_DrawPixel((uint8_t)(x + 6U), (uint8_t)(y + 12U), arrow_state);
	}
}
/*
 * Draw bottom status bar.
 *
 * Format:
 * DRV | Sxx | Txx | Wn
 */
static void OLED_DrawBottomStatusClean(const ForzaDB_Dashboard_t *pDash){
	char line[10];

	if(pDash == 0U) return;

	SSD1306_DrawRect(0U, 55U, 124U, 9U, SSD1306_PIXEL_ON);

	SSD1306_DrawString5x7(3U, 56U, "DRV");

	SSD1306_FillRect(29U, 56U, 1U, 8U, SSD1306_PIXEL_ON);

	snprintf(line, sizeof(line), "S%u", pDash->SlipPercent);
	SSD1306_DrawString5x7(35U, 56U, line);

	SSD1306_FillRect(61U, 56U, 1U, 8U, SSD1306_PIXEL_ON);

	snprintf(line, sizeof(line), "T%u", pDash->TireTempC);
	SSD1306_DrawString5x7(67U, 56U, line);

	SSD1306_FillRect(93U, 56U, 1U, 8U, SSD1306_PIXEL_ON);

	snprintf(line, sizeof(line), "W%u", pDash->WarningLevel);
	SSD1306_DrawString5x7(99U, 56U, line);

	if(pDash->WarningLevel != FORZADB_WARNING_NONE){
		SSD1306_FillRect(119U, 57U, 3U, 5U, SSD1306_PIXEL_ON);
	}
}

/*
 * Read 16-bit little-endian unsigned value from byte buffer.
 */
static uint16_t ReadU16LE(const uint8_t *pData, uint32_t offset){
	return ((uint16_t)pData[offset]) |
		   ((uint16_t)pData[offset + 1U] << 8U);
}

/*
 * Read 32-bit little-endian unsigned value from byte buffer.
 */
static uint32_t ReadU32LE(const uint8_t *pData, uint32_t offset){
	return ((uint32_t)pData[offset]) |
		   ((uint32_t)pData[offset + 1U] << 8U) |
		   ((uint32_t)pData[offset + 2U] << 16U) |
		   ((uint32_t)pData[offset + 3U] << 24U);
}

/*
 * Read 32-bit little-endian signed value from byte buffer.
 */
static int32_t ReadS32LE(const uint8_t *pData, uint32_t offset){
	return (int32_t)ReadU32LE(pData, offset);
}

/*
 * Read 32-bit little-endian float value from byte buffer.
 */
static float ReadF32LE(const uint8_t *pData, uint32_t offset){
	union{
		uint32_t u32;
		float f32;
	} value;

	value.u32 = ReadU32LE(pData, offset);

	return value.f32;
}

/*
 * Convert 0-255 input value to 0-100 percent.
 */
static uint8_t ScaleU8ToPercent(uint8_t value){
	return (uint8_t)(((uint16_t)value * 100U) / 255U);
}

/*
 * Clamp float value to percent range.
 */
static uint8_t ClampFloatToPercent(float value){
	if(value <= 0.0f){
		return 0U;
	}

	if(value >= 100.0f){
		return 100U;
	}

	return (uint8_t)value;
}

/*
 * Convert maximum absolute value of four slip inputs to 0-100 percent.
 */
static uint8_t AbsMax4ToPercent(float a, float b, float c, float d){
	float max;

	if(a < 0.0f){
		a = -a;
	}

	if(b < 0.0f){
		b = -b;
	}

	if(c < 0.0f){
		c = -c;
	}

	if(d < 0.0f){
		d = -d;
	}

	max = a;

	if(b > max){
		max = b;
	}

	if(c > max){
		max = c;
	}

	if(d > max){
		max = d;
	}

	/*
	 * Forza tire combined slip:
	 *
	 * 0.0 means grip.
	 * |slip| > 1.0 means loss of grip.
	 */
	if(max >= 1.0f){
		return 100U;
	}

	return (uint8_t)(max * 100.0f);
}

/*
 * Return maximum value of four float inputs.
 */
static float Max4Float(float a, float b, float c, float d){
	float max;

	max = a;

	if(b > max){
		max = b;
	}

	if(c > max){
		max = c;
	}

	if(d > max){
		max = d;
	}

	return max;
}
