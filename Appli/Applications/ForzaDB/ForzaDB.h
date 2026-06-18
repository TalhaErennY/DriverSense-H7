/*
 * ForzaDB.h
 *
 *  Created on: 18 Haz 2026
 *      Author: talha
 */

#ifndef FORZADB_FORZADB_H_
#define FORZADB_FORZADB_H_

#include <stdint.h>
#include "stm32h7sxx_i2c_driver.h"

/*
 * Forza UDP configuration
 */
#define FORZADB_UDP_PORT						5005U
#define FORZADB_PACKET_SIZE						324U

/*
 * Forza packet offsets
 *
 * All multi-byte fields are little-endian.
 */
#define FORZADB_OFF_ISRACEON					0U
#define FORZADB_OFF_TIMESTAMP_MS				4U

#define FORZADB_OFF_ENGINE_MAX_RPM				8U
#define FORZADB_OFF_ENGINE_IDLE_RPM				12U
#define FORZADB_OFF_ENGINE_CURRENT_RPM			16U

#define FORZADB_OFF_ACCELERATION_X				20U
#define FORZADB_OFF_ACCELERATION_Y				24U
#define FORZADB_OFF_ACCELERATION_Z				28U

#define FORZADB_OFF_VELOCITY_X					32U
#define FORZADB_OFF_VELOCITY_Y					36U
#define FORZADB_OFF_VELOCITY_Z					40U

#define FORZADB_OFF_ANGULAR_VELOCITY_X			44U
#define FORZADB_OFF_ANGULAR_VELOCITY_Y			48U
#define FORZADB_OFF_ANGULAR_VELOCITY_Z			52U

#define FORZADB_OFF_YAW							56U
#define FORZADB_OFF_PITCH						60U
#define FORZADB_OFF_ROLL						64U

#define FORZADB_OFF_NORMALIZED_SUSPENSION_FL	68U
#define FORZADB_OFF_NORMALIZED_SUSPENSION_FR	72U
#define FORZADB_OFF_NORMALIZED_SUSPENSION_RL	76U
#define FORZADB_OFF_NORMALIZED_SUSPENSION_RR	80U

#define FORZADB_OFF_TIRE_SLIP_RATIO_FL			84U
#define FORZADB_OFF_TIRE_SLIP_RATIO_FR			88U
#define FORZADB_OFF_TIRE_SLIP_RATIO_RL			92U
#define FORZADB_OFF_TIRE_SLIP_RATIO_RR			96U

#define FORZADB_OFF_TIRE_SLIP_ANGLE_FL			164U
#define FORZADB_OFF_TIRE_SLIP_ANGLE_FR			168U
#define FORZADB_OFF_TIRE_SLIP_ANGLE_RL			172U
#define FORZADB_OFF_TIRE_SLIP_ANGLE_RR			176U

#define FORZADB_OFF_TIRE_COMBINED_SLIP_FL		180U
#define FORZADB_OFF_TIRE_COMBINED_SLIP_FR		184U
#define FORZADB_OFF_TIRE_COMBINED_SLIP_RL		188U
#define FORZADB_OFF_TIRE_COMBINED_SLIP_RR		192U

#define FORZADB_OFF_SPEED						256U
#define FORZADB_OFF_POWER						260U
#define FORZADB_OFF_TORQUE						264U

#define FORZADB_OFF_TIRE_TEMP_FL				268U
#define FORZADB_OFF_TIRE_TEMP_FR				272U
#define FORZADB_OFF_TIRE_TEMP_RL				276U
#define FORZADB_OFF_TIRE_TEMP_RR				280U

#define FORZADB_OFF_BOOST						284U
#define FORZADB_OFF_FUEL						288U
#define FORZADB_OFF_DISTANCE_TRAVELED			292U

#define FORZADB_OFF_BEST_LAP					296U
#define FORZADB_OFF_LAST_LAP					300U
#define FORZADB_OFF_CURRENT_LAP					304U
#define FORZADB_OFF_CURRENT_RACE_TIME			308U
#define FORZADB_OFF_LAP_NUMBER					312U
#define FORZADB_OFF_RACE_POSITION				314U

#define FORZADB_OFF_ACCEL						315U
#define FORZADB_OFF_BRAKE						316U
#define FORZADB_OFF_CLUTCH						317U
#define FORZADB_OFF_HANDBRAKE					318U
#define FORZADB_OFF_GEAR						319U
#define FORZADB_OFF_STEER						320U

#define FORZADB_OFF_NORMALIZED_DRIVING_LINE		321U
#define FORZADB_OFF_NORMALIZED_AI_BRAKE_DIFF	322U

/*
 * Warning level selection
 */
#define FORZADB_WARNING_NONE					0U
#define FORZADB_WARNING_LOW						1U
#define FORZADB_WARNING_HIGH					2U

/*
 * Parsed Forza telemetry data
 */
typedef struct{
	int32_t  IsRaceOn;
	uint32_t TimestampMS;

	float EngineMaxRpm;
	float EngineIdleRpm;
	float CurrentEngineRpm;

	float AccelerationX;
	float AccelerationY;
	float AccelerationZ;

	float VelocityX;
	float VelocityY;
	float VelocityZ;

	float AngularVelocityX;
	float AngularVelocityY;
	float AngularVelocityZ;

	float Yaw;
	float Pitch;
	float Roll;

	float TireSlipRatioFrontLeft;
	float TireSlipRatioFrontRight;
	float TireSlipRatioRearLeft;
	float TireSlipRatioRearRight;

	float TireSlipAngleFrontLeft;
	float TireSlipAngleFrontRight;
	float TireSlipAngleRearLeft;
	float TireSlipAngleRearRight;

	float TireCombinedSlipFrontLeft;
	float TireCombinedSlipFrontRight;
	float TireCombinedSlipRearLeft;
	float TireCombinedSlipRearRight;

	float Speed;
	float Power;
	float Torque;

	float TireTempFrontLeft;
	float TireTempFrontRight;
	float TireTempRearLeft;
	float TireTempRearRight;

	float Boost;
	float Fuel;
	float DistanceTraveled;

	float BestLap;
	float LastLap;
	float CurrentLap;
	float CurrentRaceTime;

	uint16_t LapNumber;
	uint8_t  RacePosition;

	uint8_t Accel;
	uint8_t Brake;
	uint8_t Clutch;
	uint8_t HandBrake;
	uint8_t Gear;

	int8_t Steer;
	int8_t NormalizedDrivingLine;
	int8_t NormalizedAIBrakeDifference;
} ForzaDB_Packet_t;

/*
 * Reduced dashboard data
 */
typedef struct{
	uint16_t SpeedKmh;
	uint16_t Rpm;
	uint16_t MaxRpm;

	uint8_t Gear;

	uint8_t AccelPercent;
	uint8_t BrakePercent;
	uint8_t ClutchPercent;
	uint8_t HandBrakePercent;

	int8_t Steer;

	uint8_t SlipPercent;
	uint8_t FuelPercent;
	uint8_t BoostPercent;

	uint16_t BoostBarX10;
	uint16_t DistanceKmX10;
	uint8_t TireTempC;

	uint8_t WarningLevel;
} ForzaDB_Dashboard_t;

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
 * ForzaDB APIs
 */
uint8_t ForzaDB_ParsePacket(const uint8_t *pPayload, uint32_t PayloadLen, ForzaDB_Packet_t *pOut);
void ForzaDB_BuildDashboardData(const ForzaDB_Packet_t *pPacket, ForzaDB_Dashboard_t *pDash);

void OLED_I2C_GPIO_Init(void);
void OLED_I2C_Init(I2C_Handle_t *pI2CHandle);
void OLED_ShowStartupInfo(void);
void OLED_ShowNetworkStatus(const AppNetworkStatus_t *pStatus, uint8_t phy_addr);
void OLED_ShowWaitingForza(uint32_t udp_count, uint32_t forza_count);
void OLED_ShowForzaDashboard(const ForzaDB_Dashboard_t *pDash);

#endif /* FORZADB_FORZADB_H_ */
