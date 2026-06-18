/*
 * ForzaDB.c
 *
 *  Created on: 11 Haz 2026
 *      Author: talha
 */

#include "ForzaDB.h"

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
	 * Boost and fuel
	 */
	pOut->Boost = ReadF32LE(pPayload, FORZADB_OFF_BOOST);
	pOut->Fuel  = ReadF32LE(pPayload, FORZADB_OFF_FUEL);

	/*
	 * Race and player inputs
	 */
	pOut->LapNumber    = ReadU16LE(pPayload, FORZADB_OFF_LAP_NUMBER);
	pOut->RacePosition = pPayload[FORZADB_OFF_RACE_POSITION];

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

	if(pPacket->EngineMaxRpm < 0.0f){
		pDash->MaxRpm = 0U;
	} else{
		pDash->MaxRpm = (uint16_t)pPacket->EngineMaxRpm;
	}

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
	 * Boost does not have a strict 0-1 range.
	 * This is only a visual scale for dashboard.
	 */
	pDash->BoostPercent = ClampFloatToPercent(pPacket->Boost * 5.0f);

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
 * 								Private Helper Functions
 ******************************************************************************************************/
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
